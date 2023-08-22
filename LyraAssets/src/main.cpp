#define VERSION "1.00"

#include <Lyra/Lyra.h>

#include <Common/Logger.h>
#include <Common/FileSystem.h>

#include <Graphics/SDLWindow.h>

#include <Input/Input.h>

#include <imgui.h>

int main(int argc, char* argv[]) {
	lyra::init();
	ImGui::CreateContext();
	
	lyra::Window window;
	
	lyra::initFileSystem(argv);
	lyra::initInputSystem(window, ImGui::GetCurrentContext());	
}
