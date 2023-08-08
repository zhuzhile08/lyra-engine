#include <Common/FileSystem.h>

#include <fmt/core.h>
#ifdef _WIN32
#include <fmt/xchar.h>
#include <locale>
#include <codecvt>
#endif

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

#ifdef _WIN32
static constexpr const wchar* const openModeStr[6] { L"r", L"w", L"a", L"r+", L"w+", L"a+" };
#else
static constexpr const char* const openModeStr[6] { "r", "w", "a", "r+", "w+", "a+" };
#endif
static constexpr size_t bufferSize = std::max(1024, BUFSIZ);

File<char>::File(const std::filesystem::path& path, OpenMode mode, bool buffered)
	: m_path(path),
	m_buffered(buffered),
#ifdef _WIN32
	m_stream(_wfopen(globalFileSystem->absolute_path(path).c_str(), openModeStr[static_cast<size_t>(mode)]), detail::FileDeleter()) {
	if (!m_stream) throw std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(fmt::format(L"Failed to load file from path: {}!", path.c_str())));
#else
	m_stream(std::fopen(globalFileSystem->absolute_path(path).c_str(), openModeStr[static_cast<size_t>(mode)]), detail::FileDeleter()) {
	if (!m_stream) throw std::runtime_error(fmt::format("Failed to load file from path: {}!", path.c_str()));
#endif

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
		std::fflush(m_stream);
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
	std::fflush(m_stream);
	return *this;
}
int File<char>::sync() {
	return std::fflush(m_stream);
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
#ifdef _WIN32
	m_stream(_wfopen(globalFileSystem->absolute_path(path).c_str(), openModeStr[static_cast<size_t>(mode)]), detail::FileDeleter()) {
	if (!m_stream) throw std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(fmt::format(L"Failed to load file from path: {}!", path.c_str())));
#else
	m_stream(std::fopen(globalFileSystem->absolute_path(path).c_str(), openModeStr[static_cast<size_t>(mode)]), detail::FileDeleter()) {
	if (!m_stream) throw std::runtime_error(fmt::format("Failed to load file from path: {}!", path.c_str()));
#endif

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
		std::fflush(m_stream);
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
	std::fflush(m_stream);
	return *this;
}
int File<wchar>::sync() {
	return std::fflush(m_stream);
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
}

void File<wchar>::rename(const std::filesystem::path& newPath) {
	std::filesystem::rename(globalFileSystem->absolute_path(m_path), globalFileSystem->absolute_path(newPath));
}
std::filesystem::path File<wchar>::absolute_path() const {
	return globalFileSystem->absolute_path(m_path);
}

} // namespace lyra
