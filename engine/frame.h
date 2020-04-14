#pragma once

#include "common.h"
#include "camera.h"

#include "glm/glm.hpp"
#include "SDL2/SDL.h"

#include <vector>
#include <thread>
#include <string>
#include <variant>

class Frame {
public:
	struct InputEvent {
		u32 key;
		b32 state = false;
	};
	struct RenderCommand {
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec2 pos = { 0.0f, 0.0f };
		glm::vec2 scale = { 1.0f, 1.0f };
		f32 rot = 0.0f;
		u32 sprite_handle = 0;
	};
	struct SoundCommand {
		enum class Type {
			PlaySound,
			PlayMusic,
			StopMusic
		};
		struct PlaySound {
			u32 handle;
			f32 volume = 1.0f;
		};
		struct PlayMusic {
			std::string name;
			b32 fade;
		};
		struct StopMusic {
			b32 fade;
		};
		Type type;
		std::variant<PlaySound, PlayMusic, StopMusic> data;
	};
	Camera camera;
	glm::vec2 window_size = { 0.0f, 0.0f };
	std::vector<InputEvent> input_events;
	std::vector<RenderCommand> world_render_commands;
	std::vector<RenderCommand> screen_render_commands;
	std::vector<SoundCommand> sound_commands;
	glm::vec4 world_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// Just set to true when needing to toggle, the graphics object auto sets it back to false after updating.
	b32 toggle_fullscreen = false;
	void clear();
	u32 mouse_state = 0;
	u32 mouse_pos_x = 0;
	u32 mouse_pos_y = 0;
};