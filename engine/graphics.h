#pragma once

#include "common.h"
#include "frame.h"
#include "graphics_context.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Graphics {
public:
	void startup(SDL_Window* window);
	void shutdown();
	void draw(Frame& frame);
private:
	b32 m_fullscreen = false;
	glm::vec2 m_current_veiwport = { 0.0f, 0.0f };
	Frame m_frame;
	GraphicsContext* m_context = nullptr;
	std::thread m_thread;
	std::atomic<b32> m_running = false;
	std::atomic<b32> m_frame_in_flight = false;
	std::condition_variable m_frame_cond;
	std::mutex m_frame_lock;

};