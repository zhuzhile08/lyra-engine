#include "GuiElements.h"

#include "IconsCodicons.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace gui {

namespace {

template <class Lambda> void disableButton(bool& disabled, Lambda&& func) {
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
		if (ImGui::MenuItem(ICON_CI_EMPTY_WINDOW " New...")) {

		} if (ImGui::MenuItem(ICON_CI_WINDOW " Open...")) {
			
		} if (ImGui::MenuItem("Open Recent...")) {
			
		}
		ImGui::Separator();
		disableButton(m_state->unsaved, [&]() {
			if (ImGui::MenuItem(ICON_CI_SAVE " Save...")) {
				
			} 
		});
		if (ImGui::MenuItem(ICON_CI_SAVE_AS " Save as")) {
			
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Exit")) {
			*m_state->running = false;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit")) {
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
		ImGui::Separator();
		disableButton(m_state->selected, [&]() {
			if (ImGui::MenuItem(ICON_CI_DIFF_RENAMED " Rename")) {
				
			}
		});
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Build")) {
		if (ImGui::MenuItem(ICON_CI_DEBUG_START " Build")) {

		} if (ImGui::MenuItem(ICON_CI_DEBUG_RERUN " Rebuild")) {
			
		} if (ImGui::MenuItem(ICON_CI_DEBUG_RESTART " Clean")) {
			
		} 
		ImGui::Separator();
		disableButton(m_state->building, [&]() {
			if (ImGui::MenuItem(ICON_CI_CLOSE " Cancel Build")) {
				
			}
		});
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void ButtonBar::draw() {
	if (ImGui::BeginViewportSideBar("Buttons", ImGui::GetMainViewport(), ImGuiDir_Up, 30, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
		ImGui::BeginTable(
			"button_table", 
			3, 
			ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV);
		ImGui::TableNextColumn();
		if (ImGui::Button(ICON_CI_EMPTY_WINDOW)) {

		} 
		ImGui::SameLine();
		if (ImGui::Button(ICON_CI_WINDOW)) {

		} 
		ImGui::SameLine();
		disableButton(m_state->unsaved, [&]() {
			if (ImGui::Button(ICON_CI_SAVE)) {
				
			}
		});
		ImGui::TableNextColumn();
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
		ImGui::TableNextColumn();
		if (ImGui::Button(ICON_CI_DEBUG_START)) {

		} 
		ImGui::SameLine();
		if (ImGui::Button(ICON_CI_DEBUG_RERUN)) {
			
		} 
		ImGui::SameLine();
		if (ImGui::Button(ICON_CI_DEBUG_RESTART)) {
			
		} 
		ImGui::SameLine();
		disableButton(m_state->unsaved, [&]() {
			if (ImGui::Button(ICON_CI_CLOSE)) {

			}
		});
		ImGui::SetScrollHereY();
		ImGui::EndTable();
		ImGui::End();
	}
}

} // namespace gui
