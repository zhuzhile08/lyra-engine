#include "SDLImGuiRenderer.h"

#include <Graphics/Window.h>

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

namespace lyra { 

namespace renderer { 

extern Window* globalWindow;

} // namespace renderer

} // namespace lyra

SDLImGuiRenderer::SDLImGuiRenderer() :
	m_renderer(SDL_CreateRenderer(lyra::renderer::globalWindow->window, nullptr, SDL_RENDERER_PRESENTVSYNC)) {
	ImGui_ImplSDL3_InitForSDLRenderer(lyra::renderer::globalWindow->window, m_renderer);
	ImGui_ImplSDLRenderer3_Init(m_renderer);
}

SDLImGuiRenderer::~SDLImGuiRenderer() {
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	SDL_DestroyRenderer(m_renderer);
}

void SDLImGuiRenderer::beginFrame() {
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
}

void SDLImGuiRenderer::endFrame() {
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
	SDL_RenderClear(m_renderer);
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(m_renderer);
}
