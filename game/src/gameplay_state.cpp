
#include "gameplay_state.h"
#include "sprite_cache.h"
#include "lerp.h"
#include "cursor.h"

#include "sound_engine.h"

#include "glm/gtc/random.hpp"

#include <iostream>

namespace {

	b32 music_test = false;

}

void GameplayState::startup() {

	m_duck_manager = new DuckManager(m_registry);
	m_ui_manager = new UIManager();
	m_scenery_manager = new SceneryManager();
	m_duck_manager->spawnDucks(10);

}

void GameplayState::update(Frame& frame) {

	if (!music_test) {
		music_test = true;
		Frame::SoundCommand command;
		Frame::SoundCommand::PlayMusic music;
		command.type = Frame::SoundCommand::Type::PlayMusic;
		music.name = "test_music";
		command.data = music;
		frame.sound_commands.push_back(command);;
	}

	const Config& config = Config::get();

	b32 fullscreen_toggle = false;

	if (!m_moving.toggled) {

		if (m_gun.reload_timestep < m_gun.reload_delay) {
			m_gun.reload_timestep += config.delta_time;
		}

		for (const auto& e : frame.input_events) {
			if (e.key == SDL_BUTTON_RIGHT) {
				if (e.state == true) {
					m_cover.toggled = true;
				}
				else {
					m_cover.toggled = false;
				}
			}
			else if (e.key == SDL_BUTTON_LEFT && e.state == true) {
				if (m_cover.percentage < 0.33) {
					if (m_gun.current_capacity > 0) {
						WorldCursorPos hitpoint(frame);
						b32 hit = m_duck_manager->handleHitpoint(hitpoint, m_cover.percentage);
						if (hit) {
							Frame::SoundCommand command;
							command.type = Frame::SoundCommand::Type::PlaySound;
							u32 sound = SoundEngine::fetchSound("duck_hit").value();
							Frame::SoundCommand::PlaySound play;
							play.handle = sound;
							command.data = play;
							frame.sound_commands.push_back(command);
						}
						m_gun.current_capacity--;
						m_camera.is_shaking = true;
					}
				}
			}
			else if (e.key == SDL_SCANCODE_F10 && e.state == true) {
				frame.toggle_fullscreen = true;
			}
		}

		if (m_cover.toggled) {
			m_cover.percentage = lerp(m_cover.percentage, 1.0f, 0.10);
			if (m_cover.percentage > 0.6) {
				/*if (m_gun.current_capacity < m_gun.max_capacity && (m_gun.reload_delay <= m_gun.reload_timestep)) {
					m_gun.reload_timestep = 0.0f;
					m_gun.current_capacity++;
				}*/
				m_gun.current_capacity = m_gun.max_capacity;
			}
		}
		else {
			m_cover.percentage = lerp(m_cover.percentage, 0.0f, 0.10);
		}

		if (m_timer.current < 4) {
			m_ui_manager->drawLargeTimer(frame, m_timer.current);
		}

		m_duck_manager->update();
		m_duck_manager->drawDucks(frame, m_cover.percentage);

		m_scenery_manager->drawGround(frame, m_cover.percentage);
		m_scenery_manager->drawCover(frame, m_cover.percentage);

		m_ui_manager->drawCrosshair(frame);

		m_ui_manager->drawSmallTimer(frame, m_timer.current);
		if (m_duck_manager->getLivingDuckCount() > 0) {
			m_timer.timestamp += config.delta_time;
			if (m_timer.timestamp > 1.0f) {
				m_timer.timestamp -= 1.0f;
				if (m_timer.current > 0) {
					m_timer.current--;
					if (m_timer.current < 1) {
						m_life.current_count--;
						if (m_life.current_count < 1) {
							// TODO: GAME OVER
						};
						m_duck_manager->destroyDucks();
					}
				}
			}
		}

		frame.filter_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (m_duck_manager->getLivingDuckCount() < 1 && m_timer.current < 1) {
			frame.filter_color = { 1.0f, 0.0f, 0.0f, 1.0f };
		}

		if (m_duck_manager->getTotalDuckCount() < 1) {
			m_moving.toggled = true;
			m_moving.direction = !m_moving.direction;
			m_moving.paralax_percentage = 0.0f;
			m_moving.cur_move_time = 0.0f;
			m_moving.target_move_time = glm::linearRand(3.0f, 6.0f);
			m_moving.banner_showing = true;
			m_moving.banner_timestamp = 0.0f;
			m_scenery_manager->flagMoving(true);
		}

	}
	else {
		for (const auto& e : frame.input_events) {
			if (e.key == SDL_SCANCODE_F10 && e.state == true) {
				frame.toggle_fullscreen = true;
			}
		}
		m_moving.cur_move_time += config.delta_time;
		if (m_moving.cur_move_time < m_moving.target_move_time) {
			m_moving.paralax_percentage = lerp(m_moving.paralax_percentage, 1.0f, 0.10);
			m_scenery_manager->moveGround(config.delta_time, m_moving.direction);
			m_scenery_manager->drawGround(frame, m_moving.paralax_percentage);
			m_scenery_manager->drawCover(frame, m_moving.paralax_percentage);
			m_ui_manager->drawCrosshair(frame);
			m_moving.banner_timestamp += config.delta_time;
			if (m_moving.banner_timestamp >= m_moving.banner_time) {
				m_moving.banner_showing = !m_moving.banner_showing;
				m_moving.banner_timestamp -= m_moving.banner_time;
			}
			if (m_moving.banner_showing) {
				m_ui_manager->drawMovingBanner(frame);
			}
		}
		else {
			m_moving.paralax_percentage = lerp(m_moving.paralax_percentage, 0.0f, 0.10);
			m_scenery_manager->drawGround(frame, m_moving.paralax_percentage);
			m_scenery_manager->drawCover(frame, m_moving.paralax_percentage);
			m_ui_manager->drawCrosshair(frame);
			m_ui_manager->drawActionBanner(frame);
			if (m_moving.paralax_percentage <= 0.001f) {
				m_moving.toggled = false;
				m_cover.percentage = 0.0f;
				m_timer = {};
				m_scenery_manager->flagMoving(false);
				m_duck_manager->spawnDucks(10);
				m_gun.current_capacity = m_gun.max_capacity;
			}
		}
	}

	m_ui_manager->drawBullets(frame, m_gun.current_capacity);
	m_ui_manager->drawLife(frame, m_life.current_count);

	frame.camera = m_camera.camera;

	m_camera.shake_time += config.delta_time;
	if (m_camera.shake_time > 1.0f) {
		m_camera.shake_time -= 1.0f;
	}

	if (m_camera.is_shaking) {
		m_camera.cur_shake_frames++;
		frame.camera.pos.x += glm::cos(m_camera.shake_time * 10) * m_camera.shake_strength;
		frame.camera.pos.y += glm::cos(m_camera.shake_time * 15) * m_camera.shake_strength;
		if (m_camera.cur_shake_frames > m_camera.max_shake_frames) {
			m_camera.cur_shake_frames = 0;
			m_camera.is_shaking = false;
		}
	}

}

void GameplayState::shutdown() {

	delete m_duck_manager;
	m_duck_manager = nullptr;

}