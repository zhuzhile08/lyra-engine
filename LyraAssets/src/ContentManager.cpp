#include "ContentManager.h"

#include <portable-file-dialogs.h>
#include <lz4.h>
#include <stb_image.h>

#include <algorithm>

#undef min

ContentManager::ContentManager() : m_recents(lyra::Json::array_type()) {
	if (lyra::fileExists("data/recents.dat")) {
		m_recents = lyra::Json::parse(lyra::StringStream("data/recents.dat", lyra::OpenMode::readExtText, false).data());
	}
}

void ContentManager::loadProjectFile() {
	if (m_unsaved) {
		auto r = pfd::message("Unsaved Changes!", "You still have unsaved changes, do you still want to proceed?", pfd::choice::ok_cancel, pfd::icon::warning).result();
		if (r == pfd::button::cancel) return;
		m_unsaved = false;
	}

	auto f = pfd::open_file("Select a project file", ".", {"Lyra Project Files", "*.lyproj"}).result();
	if (!f.empty()) {
		m_recents.get<lyra::Json::array_type>().push_back(m_recents.insert(f[0]));
		m_projectFilePath = f[0];
		m_projectFile = lyra::Json::parse(lyra::StringStream(f[0], lyra::OpenMode::readText).data());

		m_validProject = true;
	}
}

void ContentManager::loadRecent(const std::filesystem::path& p) {
	if (std::filesystem::exists(p)) {
		m_projectFilePath = p;
		m_projectFile = lyra::Json::parse(lyra::StringStream(p, lyra::OpenMode::readText).data());

		m_validProject = true;
		m_unsaved = false;
	}
}

void ContentManager::createProjectFile() {
	if (m_unsaved) {
		auto r = pfd::message("Unsaved Changes!", "You still have unsaved changes, do you still want to proceed?", pfd::choice::ok_cancel, pfd::icon::warning).result();
		if (r == pfd::button::cancel) return;
	} 

	auto f = (pfd::select_folder("Select a folder for the project file").result()).append("/Assets.lyproj");
	if (!f.empty()) {
		m_recents.get<lyra::Json::array_type>().push_back(m_recents.insert(f[0]));
		m_projectFilePath = f;

		if (std::filesystem::exists(lyra::absolutePath(f))) {
			auto r = pfd::message("File already exsists!", "A Lyra project file already exists at the specified location, do you want to overwrite it?", pfd::choice::yes_no, pfd::icon::warning).result();
			if (r == pfd::button::no) {
				m_projectFile = lyra::Json::parse(lyra::StringStream(f, lyra::OpenMode::readExtText).data());
				return;
			}
		}

		lyra::StringStream s(f, lyra::OpenMode::writeExtText);
		if (!s.good()) {
			return;
		}
		s.write("{}", 2);
		m_projectFile = lyra::Json::parse(s.data());

		m_validProject = true;
		m_unsaved = true;
	}
}

void ContentManager::save() {
	if (m_unsaved) {
		lyra::ByteFile f(m_projectFilePath, lyra::OpenMode::writeExtText);
		auto s = m_projectFile.stringify();
		f.write(s.data(), s.size());
		m_unsaved = false;
	}
}

void ContentManager::saveAs() {
	if (m_validProject) {
		auto p = pfd::save_file("Select a path to save as the project file", ".", {"Lyra Project Files", "*.lyproj"}).result();
		if (!p.empty()) {
			lyra::ByteFile f(p, lyra::OpenMode::writeExtText);
			auto s = m_projectFile.stringify();
			f.write(s.data(), s.size());

			m_recents.get<lyra::Json::array_type>().push_back(m_recents.insert(p[0]));
			m_projectFilePath = p;

			m_validProject = true;
			m_unsaved = false;
		}
	}
}

void ContentManager::loadItem() {
	auto p = pfd::open_file("Load an item", ".", {
		"Image Files", "*.png", "*.bmp", "*.jpg", "*.jpeg", "*.psd",
		"3D Model Files", "*.obj", "*.gltf", "*.glb", "*.fbx", "*.dae", "*.blend"
		"Material Files", "*.mat",
		"True Type Fonts", "*.ttf",
		"Text/Binary Files", "*.txt", "*.json", "*.spv", "*.lua"

	}, pfd::opt::multiselect).result();

	for (const auto& f : p) {
		loadItem(f);
	}

	m_unsaved = true;
}

void ContentManager::loadFolder() {
	auto readF = [&](const std::filesystem::path& p, auto&& readF) -> void {
		for (const auto& f : std::filesystem::directory_iterator(p)) {
			if (std::filesystem::is_directory(f)) {
				readF(f, readF);
			} else {
				loadItem(f);
			}
		}
	};

	auto p = pfd::select_folder("Select a folder to load").result();
	if (!p.empty()) {
		readF(p, readF);
	}

	m_unsaved = true;
}

void ContentManager::build() {
	m_buildCancelled = false;

	lyra::uint32 i = 0;

	while (m_buildCancelled == false && i < m_newFiles.size()) {
		auto ext = m_newFiles[i].extension();
		auto concat = m_newFiles[i];
		concat.concat(".dat");

		if (ext == ".png" || ext == ".bmp" || ext == ".jpg"  || ext == ".jpeg"  || ext == ".psd") {
			int width, height, channels;
			lyra::uint8* data = stbi_load_from_file(
				lyra::ByteFile(m_newFiles[i], 
				lyra::OpenMode::readText).stream(), 
				&width, 
				&height, 
				&channels, 
				0
			);

			m_projectFile[m_newFiles[i].string()]["Width"].get<lyra::uint32>() = static_cast<lyra::uint32>(width);
			m_projectFile[m_newFiles[i].string()]["Height"].get<lyra::uint32>() = static_cast<lyra::uint32>(height);
			m_projectFile[m_newFiles[i].string()]["Mipmap"].get<lyra::uint32>() = static_cast<lyra::uint32>(std::max(static_cast<int>(std::floor(std::log2(std::max(width, height)))) - 3, 1)); 

			std::vector<char> result(LZ4_compressBound(width * height * sizeof(lyra::uint8)));
			result.resize(LZ4_compress_default(reinterpret_cast<char*>(data), result.data(), width * height * sizeof(lyra::uint8), result.size()));

			lyra::ByteFile buildFile(concat, lyra::OpenMode::writeExtBin);
			buildFile.write(
				result.data(), 
				sizeof(lyra::uint8), 
				result.size()
			);

			stbi_image_free(data);
		} else if (ext == ".fbx" || ext == ".dae" || ext == ".blend" || ext == ".obj" || ext == ".gltf" || ext == ".glb") {
		
		} else if (ext == ".mtl") {

		} else if (ext == ".ttf") {

		} else if (ext == ".ogg" || ext == ".wav") {

		}

		++i;
	}

	m_newFiles.clear();
}

void ContentManager::rebuild() {
	m_buildCancelled = false;

	clean();

	for (const auto& path : m_projectFile) {
		m_newFiles.push_back(path.first);
	}
	
	build();
}

void ContentManager::clean() {
	auto loopF = [&](const std::filesystem::path& p, auto&& loopF) -> void {
		for (const auto& f : std::filesystem::directory_iterator(p)) {
			if (f.is_directory()) {
				loopF(f, loopF);
			} else {
				auto p = f.path();
				if (p.extension() == ".dat") {
					std::filesystem::remove(p);
				}
			}
		}
	};

	auto projectDirectory = m_projectFilePath;
	loopF(lyra::absolutePath(projectDirectory.remove_filename()), loopF);
}

void ContentManager::cancel() {
	m_buildCancelled = true;
}

bool ContentManager::close() {
	if (m_unsaved) {
		auto r = pfd::message("Unsaved Changes!", "You still have unsaved changes, do you still want to proceed?", pfd::choice::ok_cancel, pfd::icon::warning).result();
		if (r == pfd::button::cancel) return true;
	}

	auto recentsFile = lyra::ByteFile("data/recents.dat", lyra::OpenMode::writeExtText, false);
	m_recents.get<lyra::Json::array_type>().resize(std::min(m_recents.get<lyra::Json::array_type>().size(), size_t(8)));
	auto stringified = m_recents.stringify();
	recentsFile.write(stringified.data(), stringified.size());

	return false;
}

void ContentManager::loadItem(const std::filesystem::path& path) {
	auto* js = &m_projectFile;
	js->insert(path.string(), js);
	js = &js->operator[](path.string());

	auto ext = path.extension();

	if (ext == ".png" || ext == ".bmp" || ext == ".jpg"  || ext == ".jpeg"  || ext == ".psd") {
		js->insert("Width", 0U);
		js->insert("Height", 0U);
		js->insert("Type", 0U);
		js->insert("Color", 0U);
		js->insert("Mipmap", 0U);
		js->insert("Dimension", 1U);
		js->insert("Wrap", 0U);
	} else if (ext == ".fbx" || ext == ".dae" || ext == ".blend" || ext == ".obj" || ext == ".gltf" || ext == ".glb") {
	
	} else if (ext == ".ttf") {

	} else if (ext == ".ogg" || ext == ".wav") {

	} else if (ext == ".lua" || ext == ".txt" || ext == ".json" || ext == ".spv" || ext == ".mtl") {

	} else if (ext == ".dat") { } // skip if it is a data file

	m_newFiles.push_back(path);
}
