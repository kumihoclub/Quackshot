
#include "graphics_context.h"

#include "glad/glad.h"
#include "glad/src/glad.c"

#include "gl_err.h"

#include <iostream>

GraphicsContext::GraphicsContext() {}

GraphicsContext::GraphicsContext(SDL_Window* window) {

	if (valid()) {
		destroy();
	}

	m_window = window;
	m_context = SDL_GL_CreateContext(m_window);

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		SDL_GL_DeleteContext(m_context);
		m_context = nullptr;
	}
	else {
		acquire();
	}

}

void GraphicsContext::release() {
	if (valid() && m_curent_owner_id == std::this_thread::get_id()) {
		if (SDL_GL_MakeCurrent(NULL, NULL) != 0) {
			std::cout << SDL_GetError() << std::endl;
		}
		else {
			m_curent_owner_id = std::thread::id();
		}
	}
}

void GraphicsContext::acquire() {
	if (valid() && m_curent_owner_id != std::this_thread::get_id()) {
		if (SDL_GL_MakeCurrent(m_window, m_context) != 0) {
			std::cout << SDL_GetError() << std::endl;
		}
		else {
			m_curent_owner_id = std::this_thread::get_id();
		}
	}
}

void GraphicsContext::swap() {
	if (valid() && m_curent_owner_id == std::this_thread::get_id()) {
		SDL_GL_SwapWindow(m_window);
	}
}

b32 GraphicsContext::valid() {
	return (m_context != nullptr);
}

void GraphicsContext::destroy() {
	if (m_context != nullptr) {
		acquire();
		SDL_GL_DeleteContext(m_context);
		m_curent_owner_id = std::thread::id();
		m_context = nullptr;
	}
}