#pragma once

#include "common.h"
#include "frame.h"

#include "SDL_mixer/SDL_mixer.h"
#include <vector>
#include <optional>
#include <string>

class SoundEngine {
public:
	void startup();
	void process(Frame& frame);
	void shutdown();
	static std::optional<u32> fetchSound(std::string name);
};