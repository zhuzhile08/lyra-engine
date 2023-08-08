#include <Common/FileSystem.h>

#include <cstdio>
#include <stdexcept>

namespace lyra {

struct FileSystem {
	NODISCARD std::filesystem::path absolute_path(const std::filesystem::path& path) const { 
		return absolutePathBase/path; 
	}

	std::filesystem::path absolutePathBase;
};

static FileSystem* globalFileSystem;

void init_filesystem(char** argv) {
	std::ios::sync_with_stdio();

	globalFileSystem = new FileSystem;

	globalFileSystem->absolutePathBase = *argv;
	globalFileSystem->absolutePathBase.remove_filename();
}

namespace detail {

void FileDeleter::operator()(FILE* ptr) const {
	std::fclose(ptr);
}

} // namespace detail

static constexpr const char* const openModeStr[6] { "r", "w", "a", "r+", "w+", "a+" };
static constexpr size_t bufferSize = std::max(1024, BUFSIZ);

File<char>::File(const std::filesystem::path& path, OpenMode mode, bool buffered)
  : m_path(path), 
 	m_buffered(buffered), 
	m_stream(std::fopen(globalFileSystem->absolute_path(path).c_str(), openModeStr[static_cast<size_t>(mode)]), detail::FileDeleter()) {
	char msg[] = "Failed to load file from path: {}!"; // dirty hack, will hopefully be gone once I implement my own logging system @todo
	if (!m_stream) throw std::runtime_error(strcat(msg, path.c_str()));

	if (m_buffered) { 
		m_buffer = m_buffer.create(bufferSize);
		std::setvbuf(m_stream, m_buffer.data(), _IOFBF, bufferSize);
	}
}
File<char>::~File<char>() {
	std::fclose(m_stream);
}
void File<char>::close() {
	std::fclose(m_stream);
}

void File<char>::disable_buffering() {
	if (m_buffered) {
		if (m_dirty) std::fflush(m_stream);
		std::setbuf(m_stream, nullptr);
	}
}
void File<char>::enable_buffering() {
	if (!m_buffered) {
		if (!m_buffer) m_buffer = m_buffer.create(bufferSize);
		std::setvbuf(m_stream, m_buffer.data(), 0, bufferSize);
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
	auto i = std::fread(string, size, count, m_stream);
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

filepos File<char>::tellg() {
	return std::ftell(m_stream);
}
filepos File<char>::tellp() {
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
File<char>& File<char>::seekp(filepos off, SeekDirection dir){
	std::fseek(m_stream, off, static_cast<int>(dir));
	return *this;
}

File<char>& File<char>::flush() {
	if (m_dirty) std::fflush(m_stream);
	return *this;
}
int File<char>::sync() {
	if (m_dirty) return std::fflush(m_stream);
	return 0;
}

bool File<char>::good() const {
	return std::ferror(m_stream) != 0;
}
bool File<char>::eof() const {
	return std::feof(m_stream) != 0;
}
void File<char>::clear() {
	std::clearerr(m_stream);
}

void File<char>::swap(File<char>& file)  {
	std::swap(flush().m_stream, file.flush().m_stream);
	m_buffer.swap(file.m_buffer);
	m_path.swap(file.m_path);
	std::swap(m_buffered, file.m_buffered);
	m_dirty = false;
	file.m_dirty = false;
}

void File<char>::rename(const std::filesystem::path& newPath) {
	std::filesystem::rename(globalFileSystem->absolute_path(m_path), globalFileSystem->absolute_path(newPath));
}
std::filesystem::path File<char>::absolute_path() const {
	return globalFileSystem->absolute_path(m_path);
}


File<wchar>::File(const std::filesystem::path& path, OpenMode mode, bool buffered)
  : m_path(path), 
 	m_buffered(buffered), 
	m_stream(std::fopen(globalFileSystem->absolute_path(path).c_str(), openModeStr[static_cast<size_t>(mode)]), detail::FileDeleter()) {
	char msg[] = "Failed to load file from path: {}!"; // dirty hack, will hopefully be gone once I implement my own logging system @todo
	if (!m_stream) throw std::runtime_error(strcat(msg, path.c_str()));

	if (m_buffered) { 
		m_buffer = m_buffer.create(bufferSize);
		std::setvbuf(m_stream, m_buffer.data(), _IOFBF, bufferSize);
	}

	std::fwide(m_stream, 1);
}
File<wchar>::~File() {
	std::fclose(m_stream);
}
void File<wchar>::close() {
	std::fclose(m_stream);
}

void File<wchar>::disable_buffering() {
	if (m_buffered) {
		if (m_dirty) std::fflush(m_stream);
		std::setbuf(m_stream, nullptr);
	}
}
void File<wchar>::enable_buffering() {
	if (!m_buffered) {
		if (!m_buffer) m_buffer = m_buffer.create(bufferSize);
		std::setvbuf(m_stream, m_buffer.data(), 0, bufferSize);
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

filepos File<wchar>::tellg() {
	return std::ftell(m_stream);
}
filepos File<wchar>::tellp() {
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
File<wchar>& File<wchar>::seekp(filepos off, SeekDirection dir){
	std::fseek(m_stream, off, static_cast<int>(dir));
	return *this;
}

File<wchar>& File<wchar>::flush() {
	if (m_dirty) std::fflush(m_stream);
	return *this;
}
int File<wchar>::sync() {
	if (m_dirty) return std::fflush(m_stream);
	return 0;
}

bool File<wchar>::good() const {
	return std::ferror(m_stream) != 0;
}
bool File<wchar>::eof() const {
	return std::feof(m_stream) != 0;
}
void File<wchar>::clear() {
	std::clearerr(m_stream);
}

void File<wchar>::swap(File& file)  {
	std::swap(flush().m_stream, file.flush().m_stream);
	m_buffer.swap(file.m_buffer);
	m_path.swap(file.m_path);
	std::swap(m_buffered, file.m_buffered);
	m_dirty = false;
	file.m_dirty = false;
}

void File<wchar>::rename(const std::filesystem::path& newPath) {
	std::filesystem::rename(globalFileSystem->absolute_path(m_path), globalFileSystem->absolute_path(newPath));
}
std::filesystem::path File<wchar>::absolute_path() const {
	return globalFileSystem->absolute_path(m_path);
}

} // namespace lyra
