#include "GuiElements.h"

#include "IconsCodicons.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

namespace gui {

namespace {

template <class Lambda> void disableButton(bool disabled, Lambda&& func) {
	if (!disabled) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	func();

	if (!disabled) {
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

			} if (ImGui::MenuItem(ICON_CI_WINDOW " Open...")) {
				
			} if (ImGui::MenuItem("Open Recent...")) {
				
			}
		});
		ImGui::Separator();
		disableButton(m_state->unsaved, [&]() {
			if (ImGui::MenuItem(ICON_CI_SAVE " Save...")) {
				
			} 
		});
		disableButton(!m_state->building, [&]() {
			if (ImGui::MenuItem(ICON_CI_SAVE_AS " Save as")) {
				
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				*m_state->running = false;
			}
		});
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit")) {
		disableButton(!m_state->building, [&]() {
			if (ImGui::BeginMenu("Add")) {
				if (ImGui::MenuItem(ICON_CI_FILE_ADD " New item...")) {

				} if (ImGui::MenuItem(ICON_CI_NEW_FOLDER " New folder...")) {
					
				}
				ImGui::Separator();
				if (ImGui::MenuItem(ICON_CI_FILE " Existing item...")) {
					
				}
				if (ImGui::MenuItem(ICON_CI_FOLDER " Existing folder...")) {
					
				}
				ImGui::EndMenu();
			}
		});
		ImGui::Separator();
		disableButton(m_state->selected, [&]() {
			if (ImGui::MenuItem(ICON_CI_DIFF_RENAMED " Rename")) m_state->rename = true;
		});
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Build")) {
		disableButton(!m_state->building, [&]() {
			if (ImGui::MenuItem(ICON_CI_DEBUG_START " Build")) {

			} if (ImGui::MenuItem(ICON_CI_DEBUG_RERUN " Rebuild")) {
				
			} if (ImGui::MenuItem(ICON_CI_DEBUG_RESTART " Clean")) {
				
			} 
		});
		ImGui::Separator();
		disableButton(m_state->building, [&]() {
			if (ImGui::MenuItem(ICON_CI_CLOSE " Cancel Build")) {
				
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
		ImGui::InputText("New name: ", &m_state->nameBuffer);

		if (ImGui::Button("Cancel...")) {
			ImGui::CloseCurrentPopup();
			m_state->nameBuffer.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("OK")) {
			ImGui::CloseCurrentPopup();
		}
		
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
				
			} 
			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_WINDOW)) {

			} 
		});
		ImGui::SameLine();
		disableButton(m_state->unsaved, [&]() {
			if (ImGui::Button(ICON_CI_SAVE)) {
				
			}
		});
		ImGui::TableNextColumn();
		disableButton(!m_state->building, [&]() {
			if (ImGui::Button(ICON_CI_FILE_ADD)) {

			} 
			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_FILE)) {

			} 
			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_NEW_FOLDER)) {
				
			} 
			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_FOLDER)) {
				
			}
		});
		ImGui::TableNextColumn();
		disableButton(!m_state->building, [&]() {
			if (ImGui::Button(ICON_CI_DEBUG_START)) {

			} 
			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_DEBUG_RERUN)) {
				
			} 
			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_DEBUG_RESTART)) {
				
			} 
		});
		ImGui::SameLine();
		disableButton(m_state->building, [&]() {
			if (ImGui::Button(ICON_CI_CLOSE)) {

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

		ImGui::End();
	}

	if (m_state->showProperties) {
		ImGui::Begin("Properties", NULL, ImGuiWindowFlags_NoCollapse);

		ImGui::End();
	}

	ImGui::End();
}

} // namespace gui
