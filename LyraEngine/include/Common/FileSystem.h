/**************************
 * @file FileSystem.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a file class responsible for writing and reading files
 * 
 * @date 2023-04-02
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Array.h>
#include <Common/SmartPointer.h>
#include <Common/Logger.h>

#include <algorithm>
#include <cstdio>
#include <iosfwd>
#include <filesystem>

namespace lyra {

namespace detail {
std::filesystem::path absolute_path(const std::filesystem::path&);
} // namespace detail

void init_filesystem(char** argv);

template <template <class...> class CTy, class LTy> class File {
public:
	enum class OpenMode {
		read = 0,
		write = 1,
		append = 2,
		read_ext = 3,
		write_ext = 4,
		append_ext = 5
	};

private:
	static constexpr const char* const openModeStr[6] { "r", "w", "a", "r+", "w+", "a+" };
	static constexpr size_t bufferSize = std::max(1024, BUFSIZ);

public:

	using literal_type = LTy;
	// note that the container type (CTy) has to be a type implemented in or using the exact style of the standard library
	using container_type = CTy<literal_type>;
	using buffer_type = CTy<char>;
	// using file_type = std::basic_fstream<literal_type>;
	using path_type = std::filesystem::path;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;

	constexpr File() = default;
	File(const path_type& path, OpenMode mode = OpenMode::read, bool buffered = true)
	 : m_path(path), m_buffered(buffered), m_stream(std::fopen(detail::absolute_path(path).c_str(), openModeStr[static_cast<size_t>(mode)])) {
		lassert(m_stream, "Failed to load file from path: ", path, "!");

		std::fseek(m_stream, 0, SEEK_END);
		m_data.resize(std::ftell(m_stream));
		std::rewind(m_stream);
		std::fread(m_data.data(), sizeof(literal_type), m_data.size(), m_stream);
		std::rewind(m_stream);

		if (m_buffered) { 
			m_buffer = m_buffer.create();
			std::setvbuf(m_stream, m_buffer->data(), _IOFBF, m_buffer->size());
		}
	}

	virtual ~File() {
		std::fclose(m_stream);
	}
	void close() {
		std::fclose(m_stream);
		m_data.clear();
		m_path.clear();
	}

	void disable_buffering() {
		if (m_buffered) {
			if (m_dirty) std::fflush(m_stream);
			std::setbuf(m_stream, nullptr);
		}
	}
	void enable_buffering() {
		if (!m_buffered) {
			if (!m_buffer) m_buffer = m_buffer.create();
			std::setvbuf(m_stream, m_buffer->data(), 0, m_buffer->size());
		}
	}

	literal_type get() {
		return std::fgetc(m_stream);
	}
	template <class STy> File& get(STy* string, size_t count) {
		std::fgets(static_cast<char*>(string), count, m_stream);
		return *this;
	}
	template <class SCTy> File& get(SCTy&& stringContainer, size_t count) {
		get(stringContainer.data(), count);
		return *this;
	}
	template <class SCTy> File& get(SCTy&& stringContainer) {
		get(stringContainer.data(), stringContainer.size());
		return *this;
	}

	template <class OCTy> File& put(OCTy c) {
		std::fputc(static_cast<char>(c), m_stream);
		auto p = std::ftell(m_stream);
		if (p > m_data.size()) m_data.push_back(static_cast<literal_type>(c));
		else m_data[p - 1] = static_cast<literal_type>(c);
		return *this;
	}
	File& write(char* string, size_t size, size_t count) {
		std::fwrite(string, size, count, m_stream);
		auto p = std::ftell(m_stream);
		for (size_t i = 0; i < count; i++) {
			if ((p - count + i) >= m_data.size()) m_data.push_back(static_cast<literal_type>(string[i]));
			else m_data[p - count + i] = static_cast<literal_type>(string[i]);
		}
		return *this;
	}
	File& write(const char* string, size_t count) {
		return write(const_cast<char*>(string), sizeof(char), count);
	}
	template <class STy> File& write(STy* string, size_t count) {
		return write(static_cast<char*>(string), sizeof(STy), count);
	}
	template <class SCTy> File& write(SCTy&& stringContainer, size_t count) {
		return write(static_cast<char*>(stringContainer.data()), sizeof(stringContainer[0]), count);
	}
	template <class SCTy> File& write(SCTy&& stringContainer) {
		return write(static_cast<char*>(stringContainer.data()), sizeof(stringContainer[0]), stringContainer.size());
	}

	void flush() {
		if (m_dirty) std::fflush(m_stream);
	}

	bool good() const {
		
	}


	fpos_t get_position() {
		return std::ftell(m_stream);
	}
	File& set_position(const fpos_t& newPos) {
		std::fsetpos(m_stream, &newPos);
		return *this;
	}
	void position_to_start(const long& offset = 0) {
		std::rewind(m_stream);
	}
	void position_to_end(const long& offset = 0) {
		std::fseek(m_stream, offset, SEEK_END);
	}
	void add_to_position(const long& offset) {
		std::fseek(m_stream, offset, SEEK_CUR);
	}

	void rename(const path_type& newPath) {
		std::filesystem::rename(detail::absolute_path(m_path), detail::absolute_path(newPath));
	} 

	void swap(const File& file) {
		std::swap(m_stream, file.m_stream);
		m_data.swap(file.m_data);
		m_path.swap(file.m_path);
	}

	void sync_data() {
		fpos_t current;
		std::fgetpos(m_stream, &current);
		std::fseek(m_stream, 0, SEEK_END);
		m_data.resize(std::ftell(m_stream));
		std::rewind(m_stream);
		std::fread(m_data.data(), sizeof(literal_type), m_data.size(), m_stream);
		std::fsetpos(m_stream, &current);
	}

	NODISCARD container_type data() const noexcept {
		return m_data;
	}
	NODISCARD path_type path() const noexcept {
		return m_path;
	}
	NODISCARD path_type absolute_path() const noexcept {
		return detail::absolute_path(m_path);
	}
	NODISCARD bool buffered() const noexcept {
		return m_buffered;
	}

private:
	std::FILE* m_stream = nullptr;
	container_type m_data;
	SmartPointer<Array<char, bufferSize>> m_buffer;

	path_type m_path;
	
	bool m_dirty;
	bool m_buffered;
};

using StringFile = File<std::basic_string, char>;
using WideStringFile = File<std::basic_string, wchar_t>;
using CharVectorFile = File<std::vector, char>;
using WideCharVectorFile = File<std::vector, wchar_t>;
using Uint8VectorFile = File<std::vector, uint8>;
using Uint16VectorFile = File<std::vector, uint16>;

} // namespace lyra
