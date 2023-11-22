#include "SDLImGuiRenderer.h"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

SDLImGuiRenderer::SDLImGuiRenderer(const lyra::Window& window) : 
	ImGuiRenderer(window), 
	m_renderer(SDL_CreateRenderer(window.get(), nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED)) {
	ImGui_ImplSDL3_InitForSDLRenderer(window.get(), m_renderer);
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
