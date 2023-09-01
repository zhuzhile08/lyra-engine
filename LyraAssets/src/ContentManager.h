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

#include <SDL_render.h>

#include <filesystem>

class ContentManager {
public:
	ContentManager();

	void loadProjectFile();
	void createProjectFile();

	void save();
	void saveAs();

	void createItem();
	void createFolder();
	void loadItem();
	void loadFolder();

	void build();
	void rebuild();
	void clean();
	void cancel();

	NODISCARD const lyra::StringStream& projectFile() const noexcept {
		return m_projectFile;
	}
	NODISCARD const lyra::ByteFile& currentAssetFile() const noexcept {
		return m_currentAssetFile;
	}
	NODISCARD const std::vector<std::filesystem::path> recents() const noexcept {
		return m_recents;
	}
	bool validProject() const noexcept {
		return m_validProject;
	}
	const bool& unsaved() const noexcept {
		return m_unsaved;
	}

private:
	lyra::StringStream m_projectFile;
	lyra::ByteFile m_currentAssetFile;

	std::vector<std::filesystem::path> m_recents;
	std::vector<std::filesystem::path> m_dirty;

	bool m_buildCancelled = false;;
	bool m_validProject = false;;
	bool m_unsaved = false;
};
