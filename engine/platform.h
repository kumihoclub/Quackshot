#pragma once

#include "common.h"
#include "frame.h"
#include "config.h"
#include "graphics_context.h"

#include "glm/glm.hpp"

class Platform {
public:
	void startup();
	void shutdown();
	void pollEvents(Frame& frame);
	GraphicsContext& getContext();
	b32 quitTriggered();
	b32 windowVisible();
	void triggerExplicitQuit();
	SDL_Window* const getWindowHandle();
private:
	b32 m_startup_success = false;
	b32 m_quit_triggered = false;
	SDL_Window* m_window = nullptr;
	GraphicsContext m_context;
	b32 m_fullscreen = false;
	glm::vec2 m_current_veiwport = { 0.0f, 0.0f };
};
