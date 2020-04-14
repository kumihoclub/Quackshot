#pragma once

#include "common.h"

#include "SDL2/SDL.h"

#include <thread>

class GraphicsContext {
public:
	GraphicsContext();
	GraphicsContext(SDL_Window* window);
	void release();
	void acquire();
	void swap();
	b32 valid();
	void destroy();
private:
	std::thread::id m_curent_owner_id;
	SDL_Window* m_window = nullptr;
	SDL_GLContext m_context = nullptr;
};