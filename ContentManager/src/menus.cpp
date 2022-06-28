#include <menus.h>

void Menus::show_window_bar() {
	bool openedAssetPopup = false;
	bool openedCompressedPopup = false;
	bool openedSettingsPopup = false;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", "Ctrl + N")) {

			} 
			
			if (ImGui::BeginMenu("Open")) {
				if (ImGui::MenuItem("Asset file", "Ctrl + O")) {
					openedAssetPopup = true;
				}

				if (ImGui::MenuItem("Compressed asset file", "Ctrl + Shift + O")) {
					openedCompressedPopup = true;
				}

				if (ImGui::MenuItem("Settings file", "Ctrl + Shift + O")) {
					openedSettingsPopup = true;
				}

				ImGui::EndMenu();
			} 
			
			if (ImGui::MenuItem("Open Recent")) {

			} 
			
			if (ImGui::MenuItem("Close", "Shift + C")) {

			} 
			
			if (ImGui::MenuItem("Exit", "Alt + F4")) {
				lyra::Application::quit();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Load")) {

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Build")) {

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (openedAssetPopup) ImGui::OpenPopup("Enter path for the asset file: ");
	if (openedCompressedPopup) ImGui::OpenPopup("Enter path for the compressed asset file: ");
	if (openedSettingsPopup) ImGui::OpenPopup("Enter path for the settings file: ");

	file_text_input("asset file");
	file_text_input("compressed asset file");
	file_text_input("settings file");
}

void Menus::file_text_input(std::string openedType) {
	char buffer[255] { };
	std::string name = "Enter path for the " + openedType + ": ";
	std::string message = "Enter a file path relative to the path of the selected " + openedType + ": ";

	if (ImGui::BeginPopupModal(name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text(message.c_str());
		ImGui::InputText("Path", buffer, 254);
		
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::SameLine();
		if (ImGui::Button("Load..."))
			
			ImGui::CloseCurrentPopup();
		
		ImGui::EndPopup();
	}
}
