
#include "sound_engine.h"
#include "config.h"

#include "nlohmann/json.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace {

	struct LoadedSound {
		std::string name;
		Mix_Chunk* data = nullptr;
	};

	struct MusicEntry {
		std::string name;
		std::string file;
	};

	std::vector<LoadedSound> loaded_sounds;
	std::vector<MusicEntry> music_entries;
	Mix_Music* active_music = nullptr;
	b32 initialized = false;

}

void SoundEngine::startup() {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) >= 0) {
		Mix_Volume(-1, 64);
		initialized = true;
		const Config& config = Config::get();
		std::string manifest_path = config.data_path + "sound/manifest.json";
		if (std::filesystem::exists(manifest_path)) {
			json sound_manifest;
			std::ifstream sound_manifest_file;
			std::stringstream sound_manifest_stream;
			sound_manifest_file.open(std::filesystem::path(manifest_path));
			sound_manifest_stream << sound_manifest_file.rdbuf();
			sound_manifest_file.close();
			sound_manifest = json::parse(sound_manifest_stream.str());
			std::vector<json> sounds = sound_manifest["sound"];
			std::vector<json> musics = sound_manifest["music"];
			for (auto& sound : sounds) {
				LoadedSound new_sound;
				new_sound.name = sound["name"];
				std::string path = config.data_path + "sound/" + std::string(sound["file"]);
				new_sound.data = Mix_LoadWAV(path.c_str());
				loaded_sounds.push_back(new_sound);
			}
			for (auto& music : musics) {
				MusicEntry new_music;
				new_music.name = music["name"];
				new_music.file = music["file"];
				music_entries.push_back(new_music);
			}
		}
	}
}

void SoundEngine::process(Frame& frame) {
	if (initialized) {
		for (auto& command : frame.sound_commands) {
			if (command.type == Frame::SoundCommand::Type::PlaySound) {
				int index = Mix_PlayChannel(-1, loaded_sounds[std::get<Frame::SoundCommand::PlaySound>(command.data).handle].data, 0);
				//f32 volume = std::get<Frame::SoundCommand::PlaySound>(command.data).volume;
				//Mix_Volume(index, 64);
			}
			else if (command.type == Frame::SoundCommand::Type::PlayMusic) {
				if (active_music != nullptr) {
					if (Mix_PlayingMusic()) {
						Mix_HaltMusic();
					}
					Mix_FreeMusic(active_music);
				}
				for (auto& entry : music_entries) {
					if (entry.name == std::get<Frame::SoundCommand::PlayMusic>(command.data).name) {
						const Config& config = Config::get();
						active_music = Mix_LoadMUS((config.data_path + "sound/" + entry.file).c_str());
						Mix_VolumeMusic(64);
						Mix_PlayMusic(active_music, -1);
					}
				}
			}
			else if (command.type == Frame::SoundCommand::Type::StopMusic) {
				if (Mix_PlayingMusic()) {
					Mix_HaltMusic();
				}
			}
		}
		frame.sound_commands.resize(0);
	}
}

void SoundEngine::shutdown() {
	if (initialized) {
		for (auto sound : loaded_sounds) {
			Mix_FreeChunk(sound.data);
		}
		Mix_FreeMusic(active_music);
	}
}

std::optional<u32> SoundEngine::fetchSound(std::string name) {
	if (initialized) {
		u32 i = 0;
		for (auto sound : loaded_sounds) {
			if (sound.name == name) {
				return i;
			}
			i++;
		}
	}
	return std::nullopt;
}