#include "GuiElements.h"

#include "IconsCodicons.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include <assimp/postprocess.h>

namespace gui {

namespace {

template <class Lambda> void disableButton(bool enabled, Lambda&& func) {
	if (!enabled) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	func();

	if (!enabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}

}

void MainMenuBar::draw() {
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		disableButton(!m_state->building, [&]() {
			if (ImGui::MenuItem(ICON_CI_EMPTY_WINDOW " New...")) {
				m_state->contentManager->createProjectFile();
			} if (ImGui::MenuItem(ICON_CI_WINDOW " Open...")) {
				m_state->contentManager->loadProjectFile();
			} if (ImGui::BeginMenu("Open Recent...")) {
				for (const auto& path : m_state->contentManager->recents().get<lyra::Json::array_type>()) {
					if (ImGui::MenuItem(path->get<lyra::Json::string_type>().c_str())) {
						m_state->contentManager->loadRecent(path->get<lyra::Json::string_type>());
					}
				}

				ImGui::EndMenu();
			}
		});
		ImGui::Separator();
		disableButton(m_state->contentManager->unsaved, [&]() {
			if (ImGui::MenuItem(ICON_CI_SAVE " Save...")) {
				m_state->contentManager->save();
			} 
		});
		disableButton(!m_state->building && m_state->contentManager->validProject(), [&]() {
			if (ImGui::MenuItem(ICON_CI_SAVE_AS " Save as")) {
				m_state->contentManager->saveAs();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				*m_state->running = false;
			}
		});
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit")) {
		disableButton(!m_state->building && m_state->contentManager->validProject(), [&]() {
			if (ImGui::BeginMenu("Add")) {
				ImGui::Separator();
				if (ImGui::MenuItem(ICON_CI_FILE " Item...")) {
					m_state->contentManager->loadItem();
				}
				if (ImGui::MenuItem(ICON_CI_FOLDER " Folder...")) {
					m_state->contentManager->loadFolder();
				}
				ImGui::EndMenu();
			}
		});
		ImGui::Separator();
		disableButton(m_state->selected && m_state->contentManager->validProject(), [&]() {
			if (ImGui::MenuItem(ICON_CI_DIFF_RENAMED " Rename")) m_state->rename = true;
		});
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Build")) {
		disableButton(!m_state->building && m_state->contentManager->validProject(), [&]() {
			if (ImGui::MenuItem(ICON_CI_DEBUG_START " Build")) {
				m_state->contentManager->build();
			} if (ImGui::MenuItem(ICON_CI_DEBUG_RERUN " Rebuild")) {
				m_state->contentManager->rebuild();
			} if (ImGui::MenuItem(ICON_CI_DEBUG_RESTART " Clean")) {
				m_state->contentManager->clean();
			} 
		});
		ImGui::Separator();
		disableButton(m_state->building && m_state->contentManager->validProject(), [&]() {
			if (ImGui::MenuItem(ICON_CI_CLOSE " Cancel Build")) {
				m_state->contentManager->cancel();
			}
		});
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View")) {
		ImGui::Checkbox("Project", &m_state->showProject);
		ImGui::Checkbox("Properties", &m_state->showProperties);
		ImGui::Checkbox("Build Console", &m_state->showConsole);
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	if (m_state->rename) ImGui::OpenPopup("Rename Item...");

	if (ImGui::BeginPopupModal("Rename Item...", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
		m_state->nameBuffer.clear();
		ImGui::InputText("New name: ", &m_state->stringBuffer);

		if (ImGui::Button("Cancel...")) {
			ImGui::CloseCurrentPopup();
			m_state->stringBuffer.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("OK")) {
			ImGui::CloseCurrentPopup();
		}
		std::filesystem::rename(m_state->contentManager->projectFilePath().remove_filename()/m_state->nameBuffer, m_state->contentManager->projectFilePath().remove_filename()/m_state->stringBuffer);
		m_state->contentManager->projectFile()[m_state->nameBuffer.string()].rename(m_state->stringBuffer);
		m_state->contentManager->unsaved = true;
		
		ImGui::EndPopup();

		m_state->rename = false;
	}
}

void ButtonBar::draw() {
	if (ImGui::BeginViewportSideBar("Buttons", ImGui::GetMainViewport(), ImGuiDir_Up, 30, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
		ImGui::BeginTable(
			"button_table", 
			3, 
			ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV);
		ImGui::TableNextColumn();
		disableButton(!m_state->building, [&]() {
			if (ImGui::Button(ICON_CI_EMPTY_WINDOW)) {
				m_state->contentManager->createProjectFile();
			} 
			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_WINDOW)) {
				m_state->contentManager->loadProjectFile();
			} 
		});
		ImGui::SameLine();
		disableButton(m_state->contentManager->unsaved && m_state->contentManager->validProject(), [&]() {
			if (ImGui::Button(ICON_CI_SAVE)) {
				m_state->contentManager->save();
			}
		});
		ImGui::TableNextColumn();
		disableButton(!m_state->building && m_state->contentManager->validProject(), [&]() {
			if (ImGui::Button(ICON_CI_FILE)) {
				m_state->contentManager->loadItem();
			} 
			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_FOLDER)) {
				m_state->contentManager->loadFolder();
			}
		});
		ImGui::TableNextColumn();
		disableButton(!m_state->building && m_state->contentManager->validProject(), [&]() {
			if (ImGui::Button(ICON_CI_DEBUG_START)) {
				m_state->contentManager->build();
			} 
			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_DEBUG_RERUN)) {
				m_state->contentManager->rebuild();
			} 
			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_DEBUG_RESTART)) {
				m_state->contentManager->clean();
			} 
		});
		ImGui::SameLine();
		disableButton(m_state->building && m_state->contentManager->validProject(), [&]() {
			if (ImGui::Button(ICON_CI_CLOSE)) {
				m_state->contentManager->cancel();
			}
		});
		ImGui::SetScrollHereY();
		ImGui::EndTable();
		ImGui::End();
	}
}

void Window::draw() {
 	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("MainWindow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking);
	ImGui::PopStyleVar();
	ImGui::DockSpace(ImGui::GetID("MenuDockSpace"));

	if (m_state->showConsole) {
		ImGui::Begin("Build Console", NULL, ImGuiWindowFlags_NoCollapse);

		ImGui::End();
	}

	if (m_state->showProject) {
		ImGui::Begin("Project", NULL, ImGuiWindowFlags_NoCollapse);

		if (m_state->contentManager->validProject()) {
			if (ImGui::TreeNode("Assets.lyproj")) {
				for (const auto& i : m_state->contentManager->projectFile()) {
					auto b = (i.first == m_state->nameBuffer);
					if (!m_state->selected) m_state->selected = b;

					if (ImGui::Selectable(i.first.c_str(), b)) {
						m_state->nameBuffer = i.first;
					}
				}

				ImGui::TreePop();
			}
		}

		ImGui::End();
	}

	if (m_state->showProperties) {
		ImGui::Begin("Properties", NULL, ImGuiWindowFlags_NoCollapse);

		if (!m_state->nameBuffer.empty()) {
			auto ext = m_state->nameBuffer.extension();
			auto& js = m_state->contentManager->projectFile()[m_state->nameBuffer.string()];

			if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
				if (ext == ".png" || ext == ".bmp" || ext == ".jpg"  || ext == ".jpeg"  || ext == ".psd") {
					static constexpr lyra::Array<const char*, 5> textureTypeComboPreview {"Texture", "Normal Map", "Light Map", "Directional Light Map", "Shadow Mask"};

					if (ImGui::BeginCombo("Type", textureTypeComboPreview[js["Type"]])) {	
						for (lyra::uint32 i = 0; i < textureTypeComboPreview.size(); i++) {
							if (ImGui::Selectable(textureTypeComboPreview[i], js["Type"] == i)) {
								js["Type"] = i;
								m_state->contentManager->unsaved = true;
							}
						}

						ImGui::EndCombo();
					} 

					if (ImGui::InputInt("Dimension", reinterpret_cast<int*>(&js["Dimension"].get<lyra::uint32>()))) {
						js["Dimension"] = std::clamp(js["Dimension"].get<lyra::uint32>(), 1U, 3U);
						m_state->contentManager->unsaved = true;
					}

					static constexpr lyra::Array<const char*, 5> textureWrapComboPreview {"Repeat", "Mirror and Repeat", "Clamp to Edge", "Clamp to Border", "Mirror and Clamp to Edge"};
					
					if (ImGui::BeginCombo("Wrap", textureWrapComboPreview[js["Wrap"]])) {	
						for (lyra::uint32 i = 0; i < textureWrapComboPreview.size(); i++) {
							if (ImGui::Selectable(textureWrapComboPreview[i], js["Wrap"] == i)) {
								js["Wrap"] = i;
								m_state->contentManager->unsaved = true;
							}
						}

						ImGui::EndCombo();
					} 

					static constexpr lyra::Array<const char*, 3> textureAlphaComboPreview {"Transparent", "Opaque Black", "Opaque White"};
					
					if (ImGui::BeginCombo("Alpha", textureAlphaComboPreview[js["Alpha"]])) {	
						for (lyra::uint32 i = 0; i < textureAlphaComboPreview.size(); i++) {
							if (ImGui::Selectable(textureAlphaComboPreview[i], js["Alpha"] == i)) {
								js["Alpha"] = i;
								m_state->contentManager->unsaved = true;
							}
						}

						ImGui::EndCombo();
					} 
				} else if (ext == ".fbx" || ext == ".dae" || ext == ".blend" || ext == ".obj" || ext == ".gltf" || ext == ".glb") {
					if (ImGui::InputInt("RotationX", reinterpret_cast<int*>(&js["RotationX"].get<lyra::uint32>()))) m_state->contentManager->unsaved = true;
					if (ImGui::InputInt("RotationY", reinterpret_cast<int*>(&js["RotationY"].get<lyra::uint32>()))) m_state->contentManager->unsaved = true;
					if (ImGui::InputInt("RotationZ", reinterpret_cast<int*>(&js["RotationZ"].get<lyra::uint32>()))) m_state->contentManager->unsaved = true;
					if (ImGui::InputInt("Scale", reinterpret_cast<int*>(&js["Scale"].get<lyra::uint32>()))) m_state->contentManager->unsaved = true;
					if (ImGui::TreeNode("ImportFlags")) {
						if (ImGui::CheckboxFlags("CalcTangentSpace", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_CalcTangentSpace)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("JoinIdenticalVertices", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_JoinIdenticalVertices)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("MakeLeftHanded", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_MakeLeftHanded)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("Triangulate", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_Triangulate)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("RemoveComponent", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_RemoveComponent)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("GenNormals", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_GenNormals)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("GenSmoothNormals", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_GenSmoothNormals)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("SplitLargeMeshes", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_SplitLargeMeshes)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("PreTransformVertices", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_PreTransformVertices)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("LimitBoneWeights", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_LimitBoneWeights)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("ValidateDataStructure", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_ValidateDataStructure)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("ImproveCacheLocality", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_ImproveCacheLocality)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("RemoveRedundantMaterials", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_RemoveRedundantMaterials)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("FixInfacingNormals", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_FixInfacingNormals)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("PopulateArmatureData", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_PopulateArmatureData)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("SortByPType", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_SortByPType)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("FindDegenerates", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_FindDegenerates)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("FindInvalidData", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_FindInvalidData)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("GenUVCoords", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_GenUVCoords)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("TransformUVCoords", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_TransformUVCoords)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("FindInstances", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_FindInstances)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("OptimizeMeshes", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_OptimizeMeshes)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("OptimizeGraph", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_OptimizeGraph)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("FlipUVs", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_FlipUVs)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("FlipWindingOrder", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_FlipWindingOrder)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("SplitByBoneCount", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_SplitByBoneCount)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("Debone", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_Debone)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("GlobalScale", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_GlobalScale)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("EmbedTextures", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_EmbedTextures)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("ForceGenNormals", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_ForceGenNormals)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("DropNormals", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_DropNormals)) m_state->contentManager->unsaved = true;
						if (ImGui::CheckboxFlags("GenBoundingBoxes", &js["ImportFlags"].get<lyra::uint32>(), aiProcess_GenBoundingBoxes)) m_state->contentManager->unsaved = true;
						ImGui::TreePop();
					}
				} else if (ext == ".ttf") {

				} else if (ext == ".ogg" || ext == ".wav") {
					
				}
			}
		}

		ImGui::End();
	}

	ImGui::End();
}

} // namespace gui
