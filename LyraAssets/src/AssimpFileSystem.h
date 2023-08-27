/*************************
 * @file AssimpFileSystem.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief An implementation of a file system to specificaly use with Assimp
 * 
 * @date 2023-08-25
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/FileSystem.h>

#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>

#include <filesystem>
#include <string>

class AssimpFile : public Assimp::IOStream {
public:
	size_t Read(void* pvBuffer, size_t pSize, size_t pCount) final {
        return std::fread(pvBuffer, pSize, pCount, m_file.stream());
    }
	size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) final {
        return std::fwrite(pvBuffer, pSize, pCount, m_file.stream());
    }
	aiReturn Seek(size_t pOffset, aiOrigin pOrigin) final {
		m_file.seekg(pOffset, static_cast<lyra::SeekDirection>(pOrigin));
		return (m_file.good()) ? aiReturn_SUCCESS : aiReturn_FAILURE;
	}
	size_t Tell() const final {
		return m_file.tellg();
	}
	size_t FileSize() const final {
        m_file.size();
    }
	void Flush () final {
		m_file.flush();
	}

protected:
	AssimpFile(const std::filesystem::path& path, const std::string& mode) : m_file(path, mode.data(), false) { }

private:
	lyra::ByteFile m_file;

	friend class AssimpFileSystem;
};

class AssimpFileSystem : public Assimp::IOSystem {
	bool Exists(const char* pFile) const final {
        lyra::doesFileExist(pFile);
    }
	char getOsSeparator() const final {
		return '/';
	}
	Assimp::IOStream* Open(const char* pFile, const char* pMode) {
		return new AssimpFile(pFile, pMode);
	}
	void Close(Assimp::IOStream* pFile) final { 
		delete pFile;
	}
};
