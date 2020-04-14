
#include "graphics.h"
#include "config.h"
#include "renderer.h"
#include "sprite_cache.h"

#include "gl_err.h"

#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>

void Graphics::startup(SDL_Window* window) {

	m_thread = std::thread([&]() -> b32 {

		SDL_Window* render_window = window;
		const Config& config = Config::get();

		SDL_GLContext context = SDL_GL_CreateContext(render_window);
		SDL_GL_MakeCurrent(render_window, context);

		gladLoadGLLoader(SDL_GL_GetProcAddress);

		if (config.enable_vsync) {
			SDL_GL_SetSwapInterval(1);
		}
		int x, y;
		SDL_GL_GetDrawableSize(render_window, &x, &y);
		glViewport(0, 0, x, y); CheckGLError();
		glEnable(GL_BLEND); CheckGLError();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); CheckGLError();

		SpriteCache::startup();
		Renderer::startup();

		m_running = true;

		while (m_running) {
			std::unique_lock<std::mutex> l(m_frame_lock);
			m_frame_cond.wait(l, [&]() {
				return m_frame_in_flight.load();
			});
			if (!m_running) {
				break;
			}
			if (m_frame.toggle_fullscreen && !m_fullscreen) {
				SDL_SetWindowFullscreen(render_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				SDL_DisplayMode current;
				SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(render_window), &current);
				m_frame.window_size = { current.w , current.h };
				//m_current_veiwport = { current.w , current.h };
				//std::cout << m_current_veiwport.x << " , " << m_current_veiwport.y << std::endl;
				m_fullscreen = true;
				m_frame.toggle_fullscreen = false;
			}
			else if (m_frame.toggle_fullscreen && m_fullscreen) {
				//SDL_ShowCursor(SDL_ENABLE);
				SDL_SetWindowFullscreen(render_window, 0);
				//m_current_veiwport = { config.viewport_size.x , config.viewport_size.y };
				//glViewport(0, 0, config.viewport_size.x, config.viewport_size.y);
				m_frame.window_size = config.viewport_size;
				m_fullscreen = false;
				m_frame.toggle_fullscreen = false;
			}
			Renderer::draw(m_frame);
			m_frame_in_flight.store(false);
			SDL_GL_SwapWindow(render_window);
		}

		Renderer::shutdown();
		SpriteCache::shutdown();

		SDL_GL_DeleteContext(context);

		return 0;

	});

	while (!m_running) {
	
	} // Busy wait until sprite everything is loaded in main thread
	int a = 0;

}

void Graphics::draw(Frame& frame) {

	b32 expected = false;

	if (m_frame_in_flight.compare_exchange_weak(expected, true)) {
		std::swap(frame, m_frame);
		m_frame_cond.notify_all();
	}

}

void Graphics::shutdown() {

	m_running = false;
	m_frame_in_flight = true;
	m_frame_cond.notify_all();
	m_thread.join();

}