#include "SDLImGuiRenderer.h"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer.h>

SDLImGuiRenderer::SDLImGuiRenderer(const lyra::Window& window) : 
	ImGuiRenderer(window), 
	m_renderer(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED)) {
	ImGui_ImplSDL2_InitForSDLRenderer(window.get(), m_renderer);
    ImGui_ImplSDLRenderer_Init(m_renderer);
}

SDLImGuiRenderer::~SDLImGuiRenderer() {
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	SDL_DestroyRenderer(m_renderer);
}

void SDLImGuiRenderer::beginFrame() {
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
}

void SDLImGuiRenderer::endFrame() {
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
	SDL_RenderClear(m_renderer);
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(m_renderer);
}
