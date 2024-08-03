/*************************
 * @file ContentManagerGuiRenderer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief The content manager
 * @brief Handles rendering and its file system
 * 
 * @date 2023-08-23
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include "IconsCodicons.h"

#include <Lyra/Lyra.h>

#include <Common/FileSystem.h>
#include <LSD/JSON.h>

#include <filesystem>

class ContentManager {
public:
	ContentManager();

	void loadProjectFile();
	void loadRecent(const std::filesystem::path& p);
	void createProjectFile();

	void save();
	void saveAs();

	void loadItem();
	void loadFolder();

	void build();
	void rebuild();
	void clean();
	void cancel();

	bool close();

	NODISCARD const lsd::Json& projectFile() const noexcept {
		return m_projectFile;
	}
	NODISCARD lsd::Json& projectFile() noexcept {
		return m_projectFile;
	}
	NODISCARD const lsd::Json& recents() const noexcept {
		return m_recents;
	}
	NODISCARD std::filesystem::path projectFilePath() const noexcept {
		return m_projectFilePath;
	}
	bool validProject() const noexcept {
		return m_validProject;
	}
	
	bool unsaved = false;

private:
	lsd::Json m_projectFile;
	lsd::Json m_recents;

	std::filesystem::path m_projectFilePath;

	lsd::Vector<std::filesystem::path> m_newFiles;
	
	bool m_buildCancelled = false;
	bool m_validProject = false;

	void loadItem(const std::filesystem::path& path);
};
