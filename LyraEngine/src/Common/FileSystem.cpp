#include <Common/FileSystem.h>
#include <Common/Logger.h>

#include <fmt/core.h>

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <unordered_set>

namespace lyra {

using PathStringType = std::filesystem::path::string_type;

static constexpr const char* const openModeStr[18] { 
	"r", "w", "a", "r+", "w+", "a+", 
	"rb", "wb", "ab", "r+b", "w+b", "a+b",
	"rt", "wt", "at", "r+t", "w+t", "a+t" };

static constexpr size_t bufferSize = std::max(1024, BUFSIZ);

struct FileSystem {
	NODISCARD std::filesystem::path absolutePath(const std::filesystem::path& path) const { 
		return absolutePathBase/path; 
	}

	NODISCARD FILE* getFile(const std::filesystem::path& path, const char* mode) {
		PathStringType p(path.native());
#ifdef _WIN32
		wchar m[4];
		std::mbstowcs(m, mode, strlen(mode));
		p.append(m);
#else
		p.append(mode);
#endif

		loadedFiles.try_emplace(
			p,
			std::fopen(absolutePath(path).string().data(), mode)
		);

		return loadedFiles.at(p);
	}

	NODISCARD char* getBuffer() {
		if (availableBuffers.empty()) {
			buffers.push_back({});
			availableBuffers.insert(buffers.back());
		}

		return availableBuffers.extract(availableBuffers.begin()).value();
	}
	
	void returnBuffer(char* buffer) {
		availableBuffers.insert(buffer);
	}

	std::vector<Array<char, bufferSize>> buffers;
	std::unordered_set<char*> availableBuffers;
	std::unordered_map<PathStringType, FILE*> loadedFiles;

	std::filesystem::path absolutePathBase;
	std::filesystem::path assetsFilePath;
};

static FileSystem* globalFileSystem;

void initFileSystem(char** argv) {
	std::ios::sync_with_stdio();

	globalFileSystem = new FileSystem;

	if (!(globalFileSystem->assetsFilePath = (globalFileSystem->absolutePathBase = *argv)).has_filename()) {
		globalFileSystem->assetsFilePath.append("Assets.lyproj");
	}

	globalFileSystem->absolutePathBase.remove_filename();
}

std::filesystem::path toGlobalPath(const std::filesystem::path& path) {
	return globalFileSystem->absolutePath(path);
}

std::filesystem::path assetsFilePath() {
	return globalFileSystem->assetsFilePath;
}


bool doesFileExist(const std::filesystem::path& path) {
	return globalFileSystem->loadedFiles.contains(path);
}

File<char>::File(const std::filesystem::path& path, OpenMode mode, bool buffered)
	: m_path(path),
	m_buffered(buffered),
	m_stream(globalFileSystem->getFile(path, openModeStr[static_cast<size_t>(mode)])) {
	if (m_buffered && m_stream) { 
		std::setvbuf(m_stream, globalFileSystem->getBuffer(), _IOFBF, bufferSize);
	}
}
File<char>::File(const std::filesystem::path& path, const char* mode, bool buffered)
	: m_path(path),
	m_buffered(buffered),
	m_stream(globalFileSystem->getFile(path, mode)) {
	if (m_buffered && m_stream) { 
		std::setvbuf(m_stream, globalFileSystem->getBuffer(), _IOFBF, bufferSize);
	}
}
File<char>::~File<char>() {
	globalFileSystem->returnBuffer(m_buffer);
}
void File<char>::close() {
	globalFileSystem->returnBuffer(m_buffer);
}

void File<char>::disableBuffering() {
	if (m_buffered) {
		std::fflush(m_stream);
		std::setbuf(m_stream, nullptr);
	}
}
void File<char>::enableBuffering() {
	if (!m_buffered) {
		std::setvbuf(m_stream, globalFileSystem->getBuffer(), _IOFBF, bufferSize);
	}
}

int File<char>::get() {
	return std::fgetc(m_stream);
}
File<char>& File<char>::get(char& c) {
	c = std::fgetc(m_stream);
	return *this;
}
File<char>& File<char>::get(char* string, size_t count) {
	std::fgets(string, count, m_stream);
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
File<char>& File<char>::read(char* string, size_t count) {
	std::fread(string, sizeof(char), count, m_stream);
	return *this;
}
File<char>& File<char>::read(void* string, size_t size, size_t count) {
	std::fread(string, size, count, m_stream);
	return *this;
}
File<char>& File<char>::put(char c) {
	std::fputc(c, m_stream);
	auto p = std::ftell(m_stream);
	return *this;
}
File<char>& File<char>::write(const char* string, size_t size, size_t count) {
	std::fwrite(string, size, count, m_stream);
	auto p = std::ftell(m_stream);
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

File<char>& File<char>::flush() {
	std::fflush(m_stream);
	return *this;
}
int File<char>::sync() {
	return std::fflush(m_stream);
	return 0;
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

void File<char>::swap(File<char>& file)  {
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


File<wchar>::File(const std::filesystem::path& path, OpenMode mode, bool buffered)
  : m_path(path), 
 	m_buffered(buffered), 
	m_stream(globalFileSystem->getFile(path, openModeStr[static_cast<size_t>(mode)])) {
	if (m_buffered && m_stream) { 
		std::setvbuf(m_stream, globalFileSystem->getBuffer(), _IOFBF, bufferSize);
	}

	std::fwide(m_stream, 1);
}
File<wchar>::File(const std::filesystem::path& path, const char* mode, bool buffered)
	: m_path(path),
	m_buffered(buffered && m_stream),
	m_stream(globalFileSystem->getFile(path, mode)) {
	if (m_buffered) { 
		std::setvbuf(m_stream, globalFileSystem->getBuffer(), _IOFBF, bufferSize);
	}

	std::fwide(m_stream, 1);
}
File<wchar>::~File() {
	globalFileSystem->returnBuffer(m_buffer);
}
void File<wchar>::close() {
	globalFileSystem->returnBuffer(m_buffer);
}

void File<wchar>::disableBuffering() {
	if (m_buffered) {
		std::fflush(m_stream);
		std::setbuf(m_stream, nullptr);
	}
}
void File<wchar>::enableBuffering() {
	if (!m_buffered) {
		std::setvbuf(m_stream, globalFileSystem->getBuffer(), _IOFBF, bufferSize);
	}
}

int File<wchar>::get() {
	return std::fgetwc(m_stream);
}
File<wchar>& File<wchar>::get(wchar& c) {
	c = std::fgetwc(m_stream);
	return *this;
}
File<wchar>& File<wchar>::get(wchar* string, size_t count) {
	std::fgetws(string, count, m_stream);
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
File<wchar>& File<wchar>::read(wchar* string, size_t count) {
	std::fread(string, sizeof(wchar), count, m_stream);
	return *this;
}
File<wchar>& File<wchar>::read(void* string, size_t size, size_t count) {
	std::fread(string, size, count, m_stream);
	return *this;
}
File<wchar>& File<wchar>::put(wchar c) {
	std::fputwc(c, m_stream);
	auto p = std::ftell(m_stream);
	return *this;
}
File<wchar>& File<wchar>::write(const wchar* string, size_t size, size_t count) {
	std::fwrite(string, size, count, m_stream);
	auto p = std::ftell(m_stream);
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
size_t File<wchar>::size() const {
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
	return 0;
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

void File<wchar>::swap(File& file)  {
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
