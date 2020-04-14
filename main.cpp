
#include "config.h"
#include "frame.h"
#include "platform.h"
#include "graphics.h"
#include "sound_engine.h"
#include "frame.h"

#include "sim.h"
#include "sprite_cache.h"

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <ratio>

#include "gl_err.h"

#define DEBUG

Config default_config;
Platform platform;
Graphics graphics;
SoundEngine sound;

Frame frame;

using timestamp = std::chrono::time_point<std::chrono::high_resolution_clock>;

int main(int argc, char *argv[]) {

	default_config.title = "Quackshot";
	default_config.viewport_size = { 960, 540 };
	default_config.unit_pixels = 32;
	default_config.update_rate = 120;
	default_config.enable_vsync = false;
#ifdef DEBUG
	default_config.data_path = "../../../data/"; // in-dev path
#else
	default_config.data_path = "data/";
#endif
	Config::set(default_config);

	const Config& config = Config::get();

	platform.startup();
	sound.startup();
	graphics.startup(platform.getWindowHandle());
	// Game systems start
	Sim::startup();
	// Game systems end

	std::chrono::duration<u64, std::nano> update_accumulator(0);
	std::chrono::duration<u64, std::milli> update_rate((u32)(config.delta_time * 1000));

	timestamp cur_time = std::chrono::high_resolution_clock::now();

	while (!platform.quitTriggered()) {
		timestamp new_time = std::chrono::high_resolution_clock::now();
		auto frame_time = std::chrono::duration<u64, std::nano>(new_time - cur_time);
		cur_time = new_time;
		update_accumulator += frame_time;
		u32 update_count = 0;
		while (update_accumulator >= update_rate) {
			frame.clear();
			platform.pollEvents(frame);

			// Game systems start
			Sim::update(frame);
			sound.process(frame);
			// Game systems end

			update_accumulator -= update_rate;
			update_count++;
			if (update_count > 3) {
				update_accumulator = std::chrono::duration<u64, std::nano>::zero();
			}
		}
		if (update_count > 0 && platform.windowVisible()) {
			graphics.draw(frame);
		}
	}

	// Game systems start
	Sim::shutdown();
	// Game systems end
	graphics.shutdown();
	sound.shutdown();
	platform.shutdown();

	return 0;
}