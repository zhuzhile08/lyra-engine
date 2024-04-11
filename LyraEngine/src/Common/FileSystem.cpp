#include <Common/FileSystem.h>

#include <Common/Logger.h>
#include <Common/SharedPointer.h>
#include <Common/Vector.h>

#include <fmt/core.h>

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <Common/UnorderedSparseMap.h>

namespace lyra {

namespace {

class FileSystem {
public:
	static constexpr size_type bufferSize = BUFSIZ;
	static constexpr size_type maxFiles = FOPEN_MAX;
	static constexpr const char* assetsFilePath = "data/Assets.lyproj";

	using PathStringType = std::filesystem::path::string_type;

	FileSystem(char** argv) : absolutePathBase(argv[0]) {
		absolutePathBase.remove_filename();
	}

	NODISCARD std::filesystem::path absolutePath(const std::filesystem::path& path) const { 
		return absolutePathBase/path; 
	}

	NODISCARD FILE* loadFile(const std::filesystem::path& path, const char* mode) {
		// @todo implement as multithreaded
		// return signal (bool mutex?)
		// run on different thread and set bool to true when done

		PathStringType p(path.native());
#ifdef _WIN32
		wchar m[4];
		std::mbstowcs(m, mode, strlen(mode));
		p.append(m);
#else
		p.append(mode);
#endif

		auto it = loadedFiles.find(p);

		if (it != loadedFiles.end()) {
			return it->second.get();
		}

		while (loadedFiles.size() >= maxFiles) {
			for (auto it = loadedFiles.begin(); it != loadedFiles.end(); it++) {
				if (it->second.count() == 1) {
					std::fflush(it->second.get());
					std::fclose(it->second.get());
					loadedFiles.erase(it);
					break;
				}
			}
		}

		auto& pair = loadedFiles.emplace(
			p,
			std::fopen(absolutePath(path).string().c_str(), mode)
		).first->second;

		ASSERT((pair.get() != nullptr), "Failed to load file at path: {}!", absolutePath(path).string());

		return pair.get();
	}

	NODISCARD char* unusedBuffer() {
		if (buffers.empty()) buffers.pushBack(new char[bufferSize]);

		auto r = buffers.back();
		buffers.popBack();
		return r;
	}
	
	void returnBuffer(char* buffer) {
		memset(buffer, '\0', bufferSize);
		buffers.pushBack(buffer);
	}

	Vector<char*> buffers;

	UnorderedSparseMap<PathStringType, SharedPointer<FILE>> loadedFiles;

	std::filesystem::path absolutePathBase;
};

const char* enumToOpenMode(OpenMode m) {
	static constexpr Array<const char*, 15> openModes {
		"rt",
		"wt",
		"at",
		"",
		"r+t",
		"w+t",
		"a+t",
		"",
		"rb",
		"wb",
		"ab",
		"",
		"r+b",
		"w+b",
		"a+b"
	};

	return openModes[static_cast<size_type>(m)];
}

}

static FileSystem* globalFileSystem;

void initFileSystem(char** argv) {
	if (globalFileSystem) {
		log::warning("lyra::initFileSystem(): The file system is already initialzed!");
		return;
	}

	std::ios::sync_with_stdio();

	globalFileSystem = new FileSystem(argv);
}

std::filesystem::path absolutePath(const std::filesystem::path& path) {
	return globalFileSystem->absolutePath(path);
}

std::filesystem::path localPath(const std::filesystem::path& path) {
	return std::filesystem::relative(path, globalFileSystem->absolutePathBase);
}

std::filesystem::path assetsFilePath() {
	return globalFileSystem->absolutePath(globalFileSystem->assetsFilePath);
}

bool fileLoaded(const std::filesystem::path& path) {
	return globalFileSystem->loadedFiles.contains(path);
}

ByteFile tmpFile() {
	auto s = 
#ifdef _WIN32
		std::to_wstring(
#else
		std::to_string(
#endif
			std::time(nullptr)); // dirty hack for the name, but who gives, aslong as it works
	globalFileSystem->loadedFiles.insert({ s, std::tmpfile() });
	return ByteFile(globalFileSystem->loadedFiles.at(s), nullptr);
}


File<char>::File(const std::filesystem::path& path, OpenMode mode, bool buffered) : 
	m_stream(globalFileSystem->loadFile(path, enumToOpenMode(mode))),
	m_path(path),
	m_buffered(buffered){
	if (m_buffered) { 
		std::setvbuf(m_stream, globalFileSystem->unusedBuffer(), _IOFBF, FileSystem::bufferSize);
	}
}
File<char>::File(const std::filesystem::path& path, const char* mode, bool buffered) : 
	m_stream(globalFileSystem->loadFile(path, mode)),
	m_path(path),
	m_buffered(buffered) {
	if (m_buffered) { 
		std::setvbuf(m_stream, globalFileSystem->unusedBuffer(), _IOFBF, FileSystem::bufferSize);
	}
}
File<char>::~File<char>() {
	if (m_buffer) globalFileSystem->returnBuffer(m_buffer);
}
void File<char>::close() {
	if (m_buffer) globalFileSystem->returnBuffer(m_buffer);
}

void File<char>::disableBuffering() {
	if (m_buffered) {
		std::fflush(m_stream);
		std::setbuf(m_stream, nullptr);
		globalFileSystem->returnBuffer(m_buffer);
	}
}
void File<char>::enableBuffering() {
	if (!m_buffered) {
		std::setvbuf(m_stream, globalFileSystem->unusedBuffer(), _IOFBF, FileSystem::bufferSize);
	}
}

int File<char>::get() {
	return std::fgetc(m_stream);
}
File<char>& File<char>::get(char& c) {
	c = std::fgetc(m_stream);
	return *this;
}
File<char>& File<char>::get(char* string, size_type count) {
	std::fgets(string, static_cast<uint32>(count), m_stream);
	return *this;
}
File<char>& File<char>::putback(int c) {
	std::ungetc(c, m_stream);
	return *this;
}
File<char>& File<char>::unget() {
	std::fseek(m_stream, -1, 0);
	return *this;
}
File<char>& File<char>::read(char* string, size_type count) {
	std::fread(string, sizeof(char), count, m_stream);
	return *this;
}
File<char>& File<char>::read(void* string, size_type size, size_type count) {
	std::fread(string, size, count, m_stream);
	return *this;
}
File<char>& File<char>::put(char c) {
	std::fputc(c, m_stream);
	return *this;
}
File<char>& File<char>::write(const void* string, size_type size, size_type count) {
	std::fwrite(string, size, count, m_stream);
	return *this;
}
File<char>& File<char>::write(const char* string, size_type count) {
	std::fwrite(string, sizeof(char), count, m_stream);
	return *this;
}

filepos File<char>::tellg() const {
	return std::ftell(m_stream);
}
filepos File<char>::tellp() const {
	return std::ftell(m_stream);
}
File<char>& File<char>::seekg(filepos pos) {
	std::fseek(m_stream, pos, SEEK_SET);
	return *this;
}
File<char>& File<char>::seekg(filepos off, SeekDirection dir) {
	std::fseek(m_stream, off, static_cast<int>(dir));
	return *this;
}
File<char>& File<char>::seekp(filepos pos) {
	std::fseek(m_stream, pos, SEEK_SET);
	return *this;
}
File<char>& File<char>::seekp(filepos off, SeekDirection dir) {
	std::fseek(m_stream, off, static_cast<int>(dir));
	return *this;
}
size_type File<char>::size() const {
	auto p = std::ftell(m_stream);
	std::fseek(m_stream, 0, SEEK_END);
	auto r = std::ftell(m_stream);
	std::fseek(m_stream, p, SEEK_SET);
	return r;
}

File<char>& File<char>::flush() {
	std::fflush(m_stream);
	return *this;
}
int File<char>::sync() {
	return std::fflush(m_stream);
}

bool File<char>::good() const {
	return m_stream;
}
bool File<char>::eof() const {
	return std::feof(m_stream) != 0;
}
void File<char>::clear() {
	std::clearerr(m_stream);
}

void File<char>::swap(File<char>& file) {
	std::swap(flush().m_stream, file.flush().m_stream);
	std::swap(m_buffer, file.m_buffer);
	m_path.swap(file.m_path);
	std::swap(m_buffered, file.m_buffered);
}

void File<char>::rename(const std::filesystem::path& newPath) {
	std::filesystem::rename(globalFileSystem->absolutePath(m_path), globalFileSystem->absolutePath(newPath));
}
std::filesystem::path File<char>::absolutePath() const {
	return globalFileSystem->absolutePath(m_path);
}


File<wchar>::File(const std::filesystem::path& path, OpenMode mode, bool buffered) : 
	m_stream(globalFileSystem->loadFile(path, enumToOpenMode(mode))), 
	m_path(path), 
 	m_buffered(buffered) {
	if (m_buffered) { 
		std::setvbuf(m_stream, globalFileSystem->unusedBuffer(), _IOFBF, FileSystem::bufferSize);
	}

	std::fwide(m_stream, 1);
}
File<wchar>::File(const std::filesystem::path& path, const char* mode, bool buffered) : 
	m_stream(globalFileSystem->loadFile(path, mode)), 
	m_path(path),
	m_buffered(buffered) {
	if (m_buffered) { 
		std::setvbuf(m_stream, globalFileSystem->unusedBuffer(), _IOFBF, FileSystem::bufferSize);
	}

	std::fwide(m_stream, 1);
}
File<wchar>::~File() {
	if (m_buffer) globalFileSystem->returnBuffer(m_buffer);
}
void File<wchar>::close() {
	if (m_buffer) globalFileSystem->returnBuffer(m_buffer);
}

void File<wchar>::disableBuffering() {
	if (m_buffered) {
		std::fflush(m_stream);
		std::setbuf(m_stream, nullptr);
	}
}
void File<wchar>::enableBuffering() {
	if (!m_buffered) {
		std::setvbuf(m_stream, globalFileSystem->unusedBuffer(), _IOFBF, FileSystem::bufferSize);
	}
}

int File<wchar>::get() {
	return std::fgetwc(m_stream);
}
File<wchar>& File<wchar>::get(wchar& c) {
	c = std::fgetwc(m_stream);
	return *this;
}
File<wchar>& File<wchar>::get(wchar* string, size_type count) {
	std::fgetws(string, static_cast<uint32>(count), m_stream);
	return *this;
}
File<wchar>& File<wchar>::putback(int c) {
	std::ungetwc(c, m_stream);
	return *this;
}
File<wchar>& File<wchar>::unget() {
	std::fseek(m_stream, -1, 0);
	return *this;
}
File<wchar>& File<wchar>::read(wchar* string, size_type count) {
	std::fread(string, sizeof(wchar), count, m_stream);
	return *this;
}
File<wchar>& File<wchar>::read(void* string, size_type size, size_type count) {
	std::fread(string, size, count, m_stream);
	return *this;
}
File<wchar>& File<wchar>::put(wchar c) {
	std::fputwc(c, m_stream);
	return *this;
}
File<wchar>& File<wchar>::write(const void* string, size_type size, size_type count) {
	std::fwrite(string, size, count, m_stream);
	return *this;
}
File<wchar>& File<wchar>::write(const wchar* string, size_type count) {
	std::fwrite(string, sizeof(wchar), count, m_stream);
	return *this;
}

filepos File<wchar>::tellg() const {
	return std::ftell(m_stream);
}
filepos File<wchar>::tellp() const {
	return std::ftell(m_stream);
}
File<wchar>& File<wchar>::seekg(filepos pos) {
	std::fseek(m_stream, pos, SEEK_SET);
	return *this;
}
File<wchar>& File<wchar>::seekg(filepos off, SeekDirection dir) {
	std::fseek(m_stream, off, static_cast<int>(dir));
	return *this;
}
File<wchar>& File<wchar>::seekp(filepos pos) {
	std::fseek(m_stream, pos, SEEK_SET);
	return *this;
}
File<wchar>& File<wchar>::seekp(filepos off, SeekDirection dir) {
	std::fseek(m_stream, off, static_cast<int>(dir));
	return *this;
}
size_type File<wchar>::size() const {
	auto p = std::ftell(m_stream);
	std::fseek(m_stream, 0, SEEK_END);
	auto r = std::ftell(m_stream);
	std::fseek(m_stream, p, SEEK_SET);
	return r;
}

File<wchar>& File<wchar>::flush() {
	std::fflush(m_stream);
	return *this;
}
int File<wchar>::sync() {
	return std::fflush(m_stream);
}

bool File<wchar>::good() const {
	return (std::ferror(m_stream) != 0) && (m_stream);
}
bool File<wchar>::eof() const {
	return std::feof(m_stream) != 0;
}
void File<wchar>::clear() {
	std::clearerr(m_stream);
}

void File<wchar>::swap(File& file) {
	std::swap(flush().m_stream, file.flush().m_stream);
	std::swap(m_buffer, file.m_buffer);
	m_path.swap(file.m_path);
	std::swap(m_buffered, file.m_buffered);
}

void File<wchar>::rename(const std::filesystem::path& newPath) {
	std::filesystem::rename(globalFileSystem->absolutePath(m_path), globalFileSystem->absolutePath(newPath));
}
std::filesystem::path File<wchar>::absolutePath() const {
	return globalFileSystem->absolutePath(m_path);
}

} // namespace lyra
