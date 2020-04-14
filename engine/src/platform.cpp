
#include "platform.h"

#include "gl_err.h"

#include "glad/glad.h"

#include "SDL_mixer\SDL_mixer.h"

#include <iostream>

namespace {

	Mix_Music* test_music = nullptr;

}

void Platform::startup() {

	const Config& config = Config::get();

	u32 init_flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;
	/*
	There is an issue with in SDL 2.0.9 on Windows where hid_enumerate() cause everything to hang up every 3 seconds or so;
	The workaround is to call SDL_Init with SDL_INIT_JOYSTICK flagged even if you are not going to support joysticks;

	Link: https://bugzilla.libsdl.org/show_bug.cgi?id=4391

	Going to do an SDL version check to check for 2.0.9 and apply the workaround if needed;
	*/
#ifdef _WIN32
	SDL_version sdl_ver;
	SDL_GetVersion(&sdl_ver);
	if (sdl_ver.patch == 9 && !(init_flags & SDL_INIT_JOYSTICK)) {
		init_flags = (init_flags | SDL_INIT_JOYSTICK);
	}
#endif
	SDL_Init(init_flags);

	/*if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cout << "FAILED TO INITIALIZE AUDIO" << std::endl;
	}*/

	//test_music = Mix_LoadMUS((config.data_path + "sound/bgm_test.wav").c_str());
	//Mix_PlayMusic(test_music, -1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);

	m_window = SDL_CreateWindow(config.title.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		config.viewport_size.x, config.viewport_size.y,
		SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

	m_current_veiwport = config.viewport_size;

	//m_context = GraphicsContext(m_window); // Acquires on creation

	/*if (m_context.valid()) {
		if (config.enable_vsync) {
			SDL_GL_SetSwapInterval(1);
		}
		glViewport(0, 0, m_current_veiwport.x, m_current_veiwport.y); CheckGLError();
		glEnable(GL_BLEND); CheckGLError();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); CheckGLError();
		glClearColor(0.0, 0.0, 0.0, 1.0); CheckGLError();
	}*/

}

void Platform::shutdown() {

	//m_context.destroy();
	//Mix_FreeMusic(test_music);
	//test_music = nullptr;
	Mix_Quit();
	SDL_Quit();
}

void Platform::pollEvents(Frame& frame) {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if ((e.type == SDL_KEYUP || e.type == SDL_KEYDOWN) && e.key.repeat == 0) { // Dont store repeat events
			Frame::InputEvent input_event;
			input_event.key = e.key.keysym.scancode;
			if (e.type == SDL_KEYDOWN) {
				input_event.state = true;
			}
			else {
				input_event.state = false;
			}
			frame.input_events.push_back(input_event);
		}
		else if ((e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN) && e.key.repeat == 0) {
			Frame::InputEvent input_event;
			input_event.key = e.button.button;
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				input_event.state = true;
			}
			else {
				input_event.state = false;
			}
			frame.input_events.push_back(input_event);
		}
		else if (e.type == SDL_QUIT) {
			m_quit_triggered = true;
		}
	}
	int x, y;
	frame.mouse_state = SDL_GetMouseState(&x, &y);
	frame.mouse_pos_x = x;
	frame.mouse_pos_y = y;
	SDL_GL_GetDrawableSize(m_window, &x, &y);
	frame.window_size = {x, y};
}

b32 Platform::quitTriggered() {
	return m_quit_triggered;
}

b32 Platform::windowVisible() {
	return !(SDL_GetWindowFlags(m_window) & SDL_WINDOW_MINIMIZED);
}

GraphicsContext& Platform::getContext() {
	return m_context;
}

SDL_Window* const Platform::getWindowHandle() {
	return m_window;
}