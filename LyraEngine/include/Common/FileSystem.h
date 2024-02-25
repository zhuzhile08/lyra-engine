/**************************
 * @file FileSystem.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief A Filesystem responsible for managing and interfacing to the engines loaded files using an standard-esque interface
 * 
 * @date 2023-04-02
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Array.h>
#include <Common/SharedPointer.h>

#include <type_traits>
#include <algorithm>
#include <filesystem>

namespace lyra {

void initFileSystem(char** argv);

enum class OpenMode {
	read,
	write,
	append,
	extend = 0x04,
	binary = 0x08
};

enum class SeekDirection {
	begin = SEEK_SET,
	current = SEEK_CUR,
	end = SEEK_END
};

enum class FileState {
	good = 0x00000000,
	bad = 0x0000001,
	fail = 0x00000002,
	eof = 0x00000004
};


template <class Ty> class File;

template<> class File<char> {
public: 
	using literal_type = char;
	using file_type = SharedPointer<std::FILE>;

	File() = default;
	File(const std::filesystem::path& path, OpenMode mode = OpenMode::read, bool buffered = true);
	File(const std::filesystem::path& path, const char* mode = "r", bool buffered = true);
	File(file_type file, char* buffer) : m_stream(file), m_buffer(buffer) { 
		if (m_buffer) enableBuffering();
	}
	~File();
	void close();

	void disableBuffering();
	void enableBuffering();

	int get();
	File& get(char& c);
	File& get(char* string, size_t count);
	File& putback(int c);
	File& unget();
	File& read(char* string, size_t count);
	File& read(void* string, size_t size, size_t count);

	File& put(char c);
	File& write(const void* string, size_t size, size_t count);
	File& write(const char* string, size_t count);

	File& flush();
	int sync();

	filepos tellg() const;
	filepos tellp() const;
	File& seekg(filepos pos);
	File& seekg(filepos off, SeekDirection dir);
	File& seekp(filepos pos);
	File& seekp(filepos off, SeekDirection dir);
	size_t size() const;

	bool good() const;
	bool eof() const;
	bool fail() const;
	bool operator!() const {
		return fail();
	}
	operator bool() const {
		return !fail();
	}
	void clear();

	void swap(File& file);

	void rename(const std::filesystem::path& newPath);
	NODISCARD std::filesystem::path absolutePath() const;
	NODISCARD std::filesystem::path path() const noexcept {
		return m_path;
	}
	NODISCARD bool buffered() const noexcept {
		return m_buffered;
	}
	NODISCARD const file_type& stream() const noexcept {
		return m_stream;
	}
	NODISCARD file_type& stream() noexcept {
		return m_stream;
	}

private:
	SharedPointer<std::FILE> m_stream = nullptr;
	char* m_buffer = nullptr;

	std::filesystem::path m_path;

	bool m_buffered;
};

template<> class File<wchar> {
public:
	using literal_type = wchar;
	using file_type = SharedPointer<std::FILE>;

	File() = default;
	File(const std::filesystem::path& path, OpenMode mode = OpenMode::read, bool buffered = true);
	File(const std::filesystem::path& path, const char* mode = "r", bool buffered = true);
	File(file_type file, char* buffer) : m_stream(file), m_buffer(buffer) { 
		if (m_buffer) enableBuffering();
	}
	~File();
	void close();

	void disableBuffering();
	void enableBuffering();

	int get();
	File& get(wchar& c);
	File& get(wchar* string, size_t count);
	File& putback(int c);
	File& unget();
	File& read(wchar* string, size_t count);
	File& read(void* string, size_t size, size_t count);

	File& put(wchar c);
	File& write(const void* string, size_t size, size_t count);
	File& write(const wchar* string, size_t count);

	File& flush();
	int sync();

	filepos tellg() const;
	filepos tellp() const;
	File& seekg(filepos pos);
	File& seekg(filepos off, SeekDirection dir);
	File& seekp(filepos pos);
	File& seekp(filepos off, SeekDirection dir);
	size_t size() const;
	
	bool good() const;
	bool eof() const;
	bool fail() const;
	bool operator!() const {
		return fail();
	}
	operator bool() const {
		return !fail();
	}
	void clear();

	void swap(File& file);

	void rename(const std::filesystem::path& newPath);
	NODISCARD std::filesystem::path absolutePath() const;
	NODISCARD std::filesystem::path path() const noexcept {
		return m_path;
	}
	NODISCARD bool buffered() const noexcept {
		return m_buffered;
	}
	NODISCARD const file_type& stream() const noexcept {
		return m_stream;
	}
	NODISCARD file_type& stream() noexcept {
		return m_stream;
	}

private:
	file_type m_stream = nullptr;
	char* m_buffer = nullptr;

	std::filesystem::path m_path;

	bool m_buffered;
};

using ByteFile = File<char>;
using WideFile = File<wchar>;


// Higher level class to read from and write to file loaded into a standard-style container
// Implements most functions found in the standard IO library, therefore slower than Byte- / WideFile
// Does not call internal file functions whilst reading, but may do so with most other functions
// Does not read from the File itself, but from an internal buffer synchronised to the current supposed state of the file
template <template<class...> class CTy, class LTy> class FileStream {
public:
	using literal_type = LTy;
	// note that the container type (CTy) has to be a type implemented in or using the pretty much exact style of the standard library
	using container_type = CTy<literal_type>;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using file_type = typename std::conditional<
		std::same_as<literal_type, char> || std::same_as<literal_type, wchar>, 
		File<literal_type>, 
		typename std::conditional<
			sizeof(literal_type) == sizeof(char), 
			File<char>, 
			File<wchar>
			>::type
		>::type; // since I only have 2 specialized overloads for File and the others are invalid, I have to do this check in order to see which type of file I have to take

	FileStream() = default;
	FileStream(const std::filesystem::path& path, OpenMode mode = OpenMode::read, bool buffered = true)
		 : m_file(path, mode, buffered) {
		if (m_file.good()) sync();
	}

	void close() {
		m_file.close();
		m_data.clear();
	}

	void disableBuffering() {
		m_file.disableBuffering();
	}
	void enableBuffering() {
		m_file.enableBuffering();
	}

	int get() {
		m_gcount = 1;
		if (++m_fpos >= m_data.size()) {
			setState(FileState::eof);
			setState(FileState::fail);
			return static_cast<int>(FileState::eof);
		} else if (m_putbackBuffer != 0) return std::exchange(m_putbackBuffer, 0);
		else return m_data[m_fpos - 1];
	}
	FileStream& get(literal_type& c) {
		c = static_cast<literal_type>(get());
		return *this;
	}
	FileStream& get(literal_type* string, size_t count, literal_type delim) {
		if (m_putbackBuffer != 0) string[0] = std::exchange(m_putbackBuffer, 0);
		else string[0] = m_data[m_fpos++];
		for (m_gcount = 1; m_gcount < std::max(count - 1, size_t(0)); m_gcount++) {
			if (m_fpos == m_data.size() - 1) {
				setState(FileState::eof);
				break;
			}
			if ((string[m_gcount] = m_data[m_fpos]) == delim) {
				m_gcount--;
				break;
			}
			m_fpos++;
		} 
		m_gcount++;
		return *this;
	}
	FileStream& get(literal_type* string, size_t count) {
		return get(string, count, '\n');
	}
	template<template<class...> class S> FileStream& get(S<literal_type>& container) {
		return get(container.data(), container.size(), '\n');
	}
	template<template<class...> class S> FileStream& get(S<literal_type>& container, literal_type delim) {
		return get(container.data(), container.size(), delim);
	}
	FileStream& getline(literal_type* string, size_t count) {
		if (m_putbackBuffer != 0) string[0] = std::exchange(m_putbackBuffer, 0);
		else string[0] = m_data[m_fpos++];
		for (m_gcount = 1; m_gcount < std::max(size_t(0), count - 1); m_gcount++) {
			if (m_fpos == m_data.size() - 1) {
				setState(FileState::eof);
				break;
			}
			if ((string[m_gcount] = m_data[m_fpos]) == '\n') {
				break;
			}
		}
		m_gcount++;
		return *this;
	}
	FileStream& getline(literal_type* string, size_t count, literal_type delim) {
		if (m_putbackBuffer != 0) string[0] = std::exchange(m_putbackBuffer, 0);
		else string[0] = m_data[m_fpos++];
		for (m_gcount = 1; m_gcount < count - std::max(size_t(0), count - 1); m_gcount++) {
			if (m_fpos == m_data.size() - 1) {
				setState(FileState::eof);
				break;
			}
			if ((string[m_gcount] = m_data[m_fpos]) == delim) {
				break;
			}
		} 
		m_gcount++;
		return *this;
	}
	FileStream& ignore(size_t count, literal_type delim) {
		for (m_gcount = 0; m_gcount > count; m_gcount++) {
			if (m_fpos == m_data.size() - 1) {
				setState(FileState::eof);
				break;
			}
			if (m_data[m_fpos++] == delim) {
				break;
			}
		}
		m_gcount++;
		return *this;
	}
	FileStream& putback(int c) {
		m_putbackBuffer = static_cast<literal_type>(c);
		return *this;
	}
	FileStream& unget() {
		m_gcount = 0;
		m_fpos--;
		return *this;
	}
	FileStream& read(literal_type* string, size_t count) {
		auto n = std::min(count, m_data.size() - m_fpos);
		if (n != count) setState(FileState::eof);
		for (m_gcount = 0; m_gcount < n; m_gcount++) string[m_gcount] = m_data(m_fpos++);
		m_gcount++;
		return *this;
	}

	FileStream& put(literal_type c) {
		m_file.seekg(m_fpos, SeekDirection::begin);
		m_file.put(static_cast<typename file_type::literal_type>(c));
		if (++m_fpos >= m_data.size()) m_data.pushBack(c);
		else m_data[m_fpos] = c;
		return *this;
	}
	FileStream& write(const literal_type* string, size_t count) {
		m_file.seekg(m_fpos, SeekDirection::begin);
		m_file.write(string, count);
		for (size_t i = 0; i < count; i++) {
			if (m_fpos >= static_cast<filepos>(m_data.size())) m_data.push_back(static_cast<literal_type>(string[i]));
			else m_data[m_fpos] = static_cast<literal_type>(string[i]);
			m_fpos++;
		}
		return *this;
	}

	filepos tellg() {
		return m_fpos;
	}
	filepos tellp() {
		return m_fpos;
	}
	FileStream& seekg(filepos pos) {
		m_fpos = pos;
		return *this;
	}
	FileStream& seekg(filepos off, SeekDirection dir) {
		switch (dir) {
			case SeekDirection::begin:
				m_fpos = off;
				break;
			case SeekDirection::current:
				m_fpos += off;
				break;
			case SeekDirection::end:
				m_fpos = m_data.size() - off - 1; // I do not know if there should be a -1, @todo
				break;
		}
		return *this;
	}
	FileStream& seekp(filepos pos) {
		m_fpos = pos;
		return *this;
	}
	FileStream& seekp(filepos off, SeekDirection dir) {
		switch (dir) {
			case SeekDirection::begin:
				m_fpos = off;
				break;
			case SeekDirection::current:
				m_fpos += off;
				break;
			case SeekDirection::end:
				m_fpos = m_data.size() - off - 1; // I do not know if there should be a -1, @todo
				break;
		}
		return *this;
	}

	bool good() const {
		return m_state == FileState::good && m_file.good();
	}
	bool eof() const {
		return (m_file.eof() || static_cast<bool>(static_cast<uint32>(m_state) & static_cast<uint32>(FileState::eof)));
	}
	bool fail() const {
		return !good();
	}
	bool operator!() const {
		return fail();
	}
	operator bool() const {
		return !fail();
	}
	void setState(FileState state) {
		// the cost of type safety (please implement enum classes with bitsets)
		m_state = static_cast<FileState>(static_cast<uint32>(m_state) | static_cast<uint32>(state));
	}
	DEPRECATED void setstate(FileState state) { // to keep standard library compatibility
		setState(state);
	}
	void clear(FileState state = FileState::good) {
		m_state = state;
		m_file.clear();
	}

	void swap(FileStream& stream) {
		m_file.swap(stream.m_file);
		m_data.swap(stream.m_data);
		std::swap(m_state, stream.m_state);
		std::swap(m_fpos, stream.m_fpos);
		std::swap(m_gcount, stream.m_gcount);
	}

	FileStream& flush() {
		m_file.flush();
		return *this;
	}
	int sync() {
		auto r = m_file.sync();
		m_data.resize(m_file.size());
		m_file.seekg(0, SeekDirection::begin);
		m_file.read(m_data.data(), sizeof(m_data[0]), m_data.size());
		m_file.seekg(m_gcount, SeekDirection::begin);
		return r;
	}

	void rename(const std::filesystem::path& newPath) {	
		m_file.rename(newPath);
	}
	NODISCARD std::filesystem::path absolutePath() const {
		m_file.absolutePath();
	}
	NODISCARD std::filesystem::path path() const noexcept {
		return m_file.m_path;
	}
	DEPRECATED NODISCARD FileState rdstate() const noexcept {
		return m_state;
	}
	NODISCARD FileState state() const noexcept {
		return m_state;
	}
	NODISCARD filepos gcount() const noexcept {
		return m_gcount;
	}
	NODISCARD bool buffered() const noexcept {
		return m_file.m_path.m_buffered;
	}
	NODISCARD const container_type& data() const noexcept {
		return m_data;
	}
	NODISCARD const file_type& loadFile() const noexcept {
		return m_file;
	}
	NODISCARD file_type& loadFile() noexcept {
		return m_file;
	}

private:
	container_type m_data;
	literal_type m_putbackBuffer = literal_type();
	file_type m_file;

	FileState m_state = FileState::good;
	filepos m_fpos = 0;
	filepos m_gcount = 0;
};

using StringStream = FileStream<std::basic_string, char>;
using WideStringStream = FileStream<std::basic_string, wchar_t>;
using CharVectorStream = FileStream<Vector, char>;
using WideCharVectorStream = FileStream<Vector, wchar_t>;
using Uint8VectorStream = FileStream<Vector, uint8>;
using Uint16VectorStream = FileStream<Vector, uint16>;

NODISCARD std::filesystem::path absolutePath(const std::filesystem::path& path);
NODISCARD std::filesystem::path localPath(const std::filesystem::path& path);
NODISCARD std::filesystem::path assetsFilePath();

inline bool fileExists(const std::filesystem::path& path) {
	return std::filesystem::exists(absolutePath(path));
}

bool fileLoaded(const std::filesystem::path& path);

NODISCARD ByteFile tmpFile();

} // namespace lyra
