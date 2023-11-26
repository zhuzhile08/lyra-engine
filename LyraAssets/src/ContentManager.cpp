#include "ContentManager.h"
#include "AssimpFileSystem.h"

#include <Common/Logger.h>

#include <portable-file-dialogs.h>
#include <lz4.h>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <algorithm>

#undef min
#undef max

ContentManager::ContentManager() : m_recents(lyra::Json::array_type()) {
	if (lyra::fileExists("data/recents.dat")) {
		m_recents = lyra::Json::parse(lyra::StringStream("data/recents.dat", lyra::OpenMode::read | lyra::OpenMode::extend, false).data());
	}
}

void ContentManager::loadProjectFile() {
	if (unsaved) {
		auto r = pfd::message("Unsaved Changes!", "You still have unsaved changes, do you still want to proceed?", pfd::choice::ok_cancel, pfd::icon::warning).result();
		if (r == pfd::button::cancel) return;
		unsaved = false;
	}

	auto f = pfd::open_file("Select a project file", ".", {"Lyra Project Files", "*.lyproj"}).result();
	if (!f.empty()) {
		lyra::log::info("Loading project file...");
		
		m_recents.get<lyra::Json::array_type>().push_back(m_recents.insert(f[0]));
		m_projectFilePath = f[0];
		m_projectFile = lyra::Json::parse(lyra::StringStream(f[0], lyra::OpenMode::read).data());

		m_validProject = true;

		lyra::log::info("Loaded project file at path: {}!", f[0]);
	}
}

void ContentManager::loadRecent(const std::filesystem::path& p) {
	if (std::filesystem::exists(p)) {
		lyra::log::info("Loading project file...");

		m_projectFilePath = p;
		m_projectFile = lyra::Json::parse(lyra::StringStream(p, lyra::OpenMode::read).data());

		m_validProject = true;
		unsaved = false;

		lyra::log::info("Loaded project file at path: {}!", p.string());
	}
}

void ContentManager::createProjectFile() {
	if (unsaved) {
		auto r = pfd::message("Unsaved Changes!", "You still have unsaved changes, do you still want to proceed?", pfd::choice::ok_cancel, pfd::icon::warning).result();
		if (r == pfd::button::cancel) return;
	} 

	auto f = pfd::select_folder("Select a folder for the project file").result();
	if (!f.empty()) {
		f.append("/Assets.lyproj");
		lyra::log::info("Creating new project file...");

		m_recents.get<lyra::Json::array_type>().push_back(m_recents.insert(f));
		m_projectFilePath = f;

		if (std::filesystem::exists(lyra::absolutePath(f))) {
			auto r = pfd::message("File already exsists!", "A Lyra project file already exists at the specified location, do you want to overwrite it?", pfd::choice::yes_no, pfd::icon::warning).result();
			if (r == pfd::button::no) {
				m_projectFile = lyra::Json::parse(lyra::StringStream(f, lyra::OpenMode::read | lyra::OpenMode::extend).data());
				return;
			}
		}

		lyra::StringStream s(f, lyra::OpenMode::write | lyra::OpenMode::extend);
		if (!s.good()) {
			return;
		}
		s.write("{}", 2);
		m_projectFile = lyra::Json::parse(s.data());

		m_validProject = true;
		unsaved = true;

		lyra::log::info("Loaded project file at path: {}!", f);
	}
}

void ContentManager::save() {
	if (unsaved) {
		lyra::log::info("Saving current project file...");

		lyra::ByteFile f(m_projectFilePath, lyra::OpenMode::write | lyra::OpenMode::extend);
		auto s = m_projectFile.stringify();
		f.write(s.data(), s.size());
		unsaved = false;

		lyra::log::info("Successfully saved current project file at path: {}!", m_projectFilePath.string());
	}
}

void ContentManager::saveAs() {
	if (m_validProject) {
		auto p = pfd::save_file("Select a path to save as the project file", ".", {"Lyra Project Files", "*.lyproj"}).result();
		if (!p.empty()) {
			lyra::log::info("Saving current project file to new file...");

			lyra::ByteFile f(p, lyra::OpenMode::write | lyra::OpenMode::extend);
			auto s = m_projectFile.stringify();
			f.write(s.data(), s.size());

			m_recents.get<lyra::Json::array_type>().push_back(m_recents.insert(p[0]));
			m_projectFilePath = p;

			m_validProject = true;
			unsaved = false;

			lyra::log::info("Successfully saved current project file to path: {}!", m_projectFilePath.string());
		}
	}
}

void ContentManager::loadItem() {
	auto p = pfd::open_file("Load an item", ".", {
		"Image Files", "*.png *.bmp *.jpg *.jpeg *.psd",
		"3D Model Files", "*.obj *.gltf *.glb *.fbx *.dae *.blend",
		"Material Files", "*.mat",
		"True Type Fonts", "*.ttf",
		"Text/Binary Files", "*.txt *.json *.spv *.lua"
	}, pfd::opt::multiselect).result();

	for (const auto& f : p) {
		loadItem(f);
	}

	unsaved = true;
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

	unsaved = true;
}

void ContentManager::build() {
	lyra::log::info("Starting Build...");

	m_buildCancelled = false;

	lyra::uint32 i = 0;

	while (m_buildCancelled == false && i < m_newFiles.size()) {
		auto ext = m_newFiles[i].extension();
		auto filepath = std::filesystem::path(m_projectFilePath).remove_filename() /= m_newFiles[i];
		auto concat = filepath;
		auto* js = &m_projectFile[m_newFiles[i].string()];
		concat.concat(".dat");

		if (ext == ".png" || ext == ".bmp" || ext == ".jpg" || ext == ".jpeg" || ext == ".psd") {
			lyra::log::debug("\tTexture: {}", filepath.string());

			int width, height, channels;
			lyra::uint8* data = stbi_load_from_file(
				lyra::ByteFile(filepath, 
				lyra::OpenMode::read | lyra::OpenMode::binary).stream(),
				&width, 
				&height, 
				&channels, 
				0
			);

			js->operator[]("Width").get<lyra::uint32>() = static_cast<lyra::uint32>(width);
			js->operator[]("Height").get<lyra::uint32>() = static_cast<lyra::uint32>(height);
			js->operator[]("Mipmap").get<lyra::uint32>() = static_cast<lyra::uint32>(std::max(static_cast<int>(std::floor(std::log2(std::max(width, height)))) - 3, 1)); 

			std::vector<char> result(LZ4_compressBound(width * height * sizeof(lyra::uint8)));
			result.resize(LZ4_compress_default(reinterpret_cast<char*>(data), result.data(), width * height * sizeof(lyra::uint8), static_cast<lyra::uint32>(result.size())));

			lyra::ByteFile buildFile(concat, lyra::OpenMode::write | lyra::OpenMode::binary);
			buildFile.write(
				result.data(), 
				sizeof(lyra::uint8), 
				result.size()
			);

			stbi_image_free(data);
		} else if (ext == ".fbx" || ext == ".dae" || ext == ".blend" || ext == ".obj" || ext == ".gltf" || ext == ".glb") {
			lyra::log::debug("\tModel: {}", filepath.string());

			Assimp::Importer importer;
			importer.SetIOHandler(new AssimpFileSystem);

			const aiScene* scene = importer.ReadFile(filepath.string(), m_projectFile[m_newFiles[i].string()]["ImportFlags"].get<lyra::uint32>());

			std::vector<lyra::Array<lyra::Array<float, 3>, 4>> vertexBlocks;
			std::vector<lyra::uint32> indexBlock;

			for (lyra::uint32 i = 0; i < scene->mNumMeshes; i++) {
				const auto* mesh = scene->mMeshes[i];
				for (lyra::uint32 j = 0; j < mesh->mNumVertices; j++) {
					vertexBlocks.push_back({{
						{{
							mesh->mVertices[j].x,
							mesh->mVertices[j].y,
							mesh->mVertices[j].z
						}},
						{{
							(mesh->HasNormals()) ? mesh->mNormals[j].x : 0.0f,
							(mesh->HasNormals()) ? mesh->mNormals[j].z : 0.0f,
							(mesh->HasNormals()) ? mesh->mNormals[j].y : 0.0f
						}},
						{{
							(mesh->HasVertexColors(0)) ? mesh->mColors[0][j].r : 0.0f,
							(mesh->HasVertexColors(0)) ? mesh->mColors[0][j].g : 0.0f,
							(mesh->HasVertexColors(0)) ? mesh->mColors[0][j].b : 0.0f
						}},
						{{
							(mesh->HasTextureCoords(0)) ? mesh->mTextureCoords[0][j].x : 0.0f,
							(mesh->HasTextureCoords(0)) ? mesh->mTextureCoords[0][j].z : 0.0f,
							static_cast<float>(mesh->mMaterialIndex)
						}}
					}});
				}

				lyra::uint32 indices = 0;

				for (lyra::uint32 j = 0; j < mesh->mNumFaces; j++) {
					const auto* face = &mesh->mFaces[j];
					indices += face->mNumIndices;
					for (lyra::uint32 k = 0; k < face->mNumIndices; k++) {
						indexBlock.push_back(face->mIndices[k]);
					}
				}

				js->operator[]("VertexBlocks").get<lyra::Json::array_type>().push_back(js->operator[]("VertexBlocks").insert(static_cast<lyra::uint32>(mesh->mNumVertices)));
				js->operator[]("IndexBlocks").get<lyra::Json::array_type>().push_back(js->operator[]("IndexBlocks").insert(static_cast<lyra::uint32>(indices)));
			}

			auto totalSize = vertexBlocks.size() * sizeof(float) * 3 * 4 + indexBlock.size() * sizeof(lyra::uint8);

			std::vector<char> data(totalSize);
			std::memcpy(&(*data.begin()), vertexBlocks.data(), vertexBlocks.size() * sizeof(float) * 3 * 4);
			std::memcpy(&(*data.begin()) + vertexBlocks.size() * sizeof(float) * 3 * 4, indexBlock.data(), indexBlock.size() * sizeof(lyra::uint8));
			
			std::vector<char> result(LZ4_compressBound(static_cast<lyra::uint32>(data.size())));
			result.resize(LZ4_compress_default(data.data(), result.data(), static_cast<lyra::uint32>(data.size()), static_cast<lyra::uint32>(result.size())));

			lyra::ByteFile buildFile(concat, lyra::OpenMode::write | lyra::OpenMode::binary);

			buildFile.write(
				result.data(), 
				sizeof(lyra::uint8), 
				result.size()
			);
		} else if (ext == ".ttf") {
			
		} else if (ext == ".ogg" || ext == ".wav") {

		}

		++i;
	}

	m_newFiles.clear();
	unsaved = true;

	lyra::log::info("Build successful!");
}

void ContentManager::rebuild() {
	m_buildCancelled = false;

	clean();

	m_newFiles.clear();
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
	if (unsaved) {
		auto r = pfd::message("Unsaved Changes!", "You still have unsaved changes, do you still want to proceed?", pfd::choice::ok_cancel, pfd::icon::warning).result();
		if (r == pfd::button::cancel) return true;
	}

	auto recentsFile = lyra::ByteFile("data/recents.dat", lyra::OpenMode::write | lyra::OpenMode::extend, false);
	m_recents.get<lyra::Json::array_type>().resize(std::min(m_recents.get<lyra::Json::array_type>().size(), size_t(8)));
	auto stringified = m_recents.stringify();
	recentsFile.write(stringified.data(), stringified.size());

	return false;
}

void ContentManager::loadItem(const std::filesystem::path& path) {
	auto* js = &m_projectFile;
	auto rel = std::filesystem::relative(path, std::filesystem::path(m_projectFilePath).remove_filename());
	auto ext = path.extension();

	if (ext == ".dat" || ext == ".mtl") { 
		return;
	} 
	
	js = js->insert(rel.string(), js)->second;

	if (ext == ".png" || ext == ".bmp" || ext == ".jpg" || ext == ".jpeg" || ext == ".psd") {
		js->insert("Width", 0U);
		js->insert("Height", 0U);
		js->insert("Type", 0U);
		js->insert("Alpha", 0U);
		js->insert("Mipmap", 0U);
		js->insert("Dimension", 1U);
		js->insert("Wrap", 0U);
	} else if (ext == ".fbx" || ext == ".dae" || ext == ".blend" || ext == ".obj" || ext == ".gltf" || ext == ".glb") {
		js->insert("ImportFlags", 0U);
		js->insert("RotationX", 0U);
		js->insert("RotationY", 0U);
		js->insert("RotationZ", 0U);
		js->insert("Scale", 1U);
		js->insert("VertexBlocks", lyra::Json::array_type());
		js->insert("IndexBlocks", lyra::Json::array_type());
	} else if (ext == ".ttf") {

	} else if (ext == ".ogg" || ext == ".wav") {

	} else if (ext == ".spv") {
		js->insert("Type", 1U);
	} else if (ext == ".lua" || ext == ".txt" || ext == ".json") {

	} 

	m_newFiles.push_back(rel);
}
