#pragma once

#include "common.h"
#include "config.h"

#include <string>
#include <optional>

namespace SpriteCache {

	void startup();
	void shutdown();
	std::optional<u32> fetchSpriteHandle(std::string name);
	std::optional<u32> getTexture();
	std::optional<u32> getSSBO();

}