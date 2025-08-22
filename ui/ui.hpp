#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "SDL.h"

#ifdef _WIN32
#include <windows.h> // SetProcessDPIAware()
#endif

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#include "memory.hpp"

class UI {
public:
    UI() = default;

    int setup(Memory<word_t>* mem_ptr);
    void run();
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    ImGuiIO& io;

    bool done = false;
	bool file_ready = false;

    // memory
    Memory<word_t>* mem_ptr;
	const int bytes_row = 16;
	const int rows = 16;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};