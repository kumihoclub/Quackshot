
#include "sprite_cache.h"
#include "config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "nlohmann/json.hpp"
#include "glad/glad.h"
#include "gl_err.h"

#include <unordered_map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>

using json = nlohmann::json;

namespace {
	
	struct Sprite {
		glm::vec4 uv = { 0.0f, 0.0f, 0.0f, 0.0f };
	};
	std::unordered_map<std::string, u32> sprites;
	std::optional<u32> cache_texture, cache_ssbo;

}

void SpriteCache::startup() {

	stbi_set_flip_vertically_on_load(true);
	auto& config = Config::get();
	std::string atlas_texture_path = config.data_path + "atlas/atlas.png";
	std::string atlas_json_path = config.data_path + "atlas/atlas.json";
	if (std::filesystem::exists(atlas_texture_path) && std::filesystem::exists(atlas_json_path)) {

		json atlas_json;

		{
			std::ifstream atlas_json_file;
			std::stringstream atlas_json_stream;
			atlas_json_file.open(std::filesystem::path(atlas_json_path));
			atlas_json_stream << atlas_json_file.rdbuf();
			atlas_json_file.close();
			atlas_json = json::parse(atlas_json_stream.str());
		}

		std::vector<json> sprite_frames = atlas_json["frames"];
		std::vector<Sprite> sprite_ssbo_staging_buffer;
		sprite_ssbo_staging_buffer.reserve(sprite_frames.size());

		for (auto& sprite : sprite_frames) {
			std::string name = sprite["filename"];
			u32 x = sprite["frame"]["x"];
			u32 y = sprite["frame"]["y"];
			u32 w = sprite["frame"]["w"];
			u32 h = sprite["frame"]["h"];

			Sprite new_sprite;
			new_sprite.uv = { (f32)x, (f32)y, (f32)w, (f32)h };

			sprite_ssbo_staging_buffer.push_back(new_sprite);

			sprites[name] = { static_cast<u32>(sprite_ssbo_staging_buffer.size() - 1) };

		}

		u32 texture, buffer;
		glGenBuffers(1, &buffer); CheckGLError();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer); CheckGLError();
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Sprite) * sprite_ssbo_staging_buffer.size(), sprite_ssbo_staging_buffer.data(), GL_STATIC_READ); CheckGLError();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); CheckGLError();
		cache_ssbo = buffer;

		int x, y, c;
		unsigned char* texture_data = stbi_load(atlas_texture_path.c_str(), &x, &y, &c, 0);

		glGenTextures(1, &texture); CheckGLError();
		glBindTexture(GL_TEXTURE_2D, texture); CheckGLError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); CheckGLError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); CheckGLError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data); CheckGLError();
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(texture_data);
		cache_texture = texture;

	}

}

void SpriteCache::shutdown() {

	if (cache_ssbo.has_value()) {
		glDeleteBuffers(1, &cache_ssbo.value());
	}
	if (cache_texture.has_value()) {
		glDeleteTextures(1, &cache_texture.value());
	}

}

std::optional<u32> SpriteCache::fetchSpriteHandle(std::string name) {

	if (sprites.find(name) != sprites.end()) {
		u32 handle = sprites[name];
		return handle;
	}
	return std::nullopt;

}

std::optional<u32> SpriteCache::getTexture() {

	return cache_texture;

}

std::optional<u32> SpriteCache::getSSBO() {

	return cache_ssbo;

}