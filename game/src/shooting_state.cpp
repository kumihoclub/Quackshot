
#include "shooting_state.h"
#include "sound_engine.h"
#include "config.h"
#include "lerp.h"

#include <iostream>

ShootingState::ShootingState(SceneryManager& scenery, UIManager& ui) {
	m_scenery = &scenery;
	m_ui = &ui;
	m_ducks = new DuckManager();

	m_sound_hit = SoundEngine::fetchSound("duck_hit").value();
	m_sound_vo_reload = SoundEngine::fetchSound("vo_reload").value();
	m_sound_gun_reload = SoundEngine::fetchSound("gun_reload").value();
	m_sound_gunshot = SoundEngine::fetchSound("gunshot").value();
	m_sound_explosion = SoundEngine::fetchSound("explosion").value();

};

ShootingState::~ShootingState() {
	delete m_ducks;
}

b32 ShootingState::update(Frame& frame) {

	for (const auto& e : frame.input_events) {
		if (e.key == SDL_SCANCODE_F10 && e.state == true) {
			frame.toggle_fullscreen = true;
		}
	}

	if (m_state == State::MainMenuStart || m_state == State::MainMenu || m_state == State::MainMenuEnd) {
		mainMenuUpdate(frame);
	}
	else if (m_state == State::Shooting) {
		shootingUpdate(frame);
	}
	else if (m_state == State::MovingStart || m_state == State::Moving || m_state == State::MovingEnd) {
		movingUpdate(frame);
	}
	else if (m_state == State::GameOverStart || m_state == State::GameOver || m_state == State::GameOverEnd) {
		gameOverUpdate(frame);
	}

	auto& config = Config::get();

	frame.camera = {};
	m_camera_modifier += config.delta_time;
	if (m_camera_modifier > 1.0f) {
		m_camera_modifier -= 1.0f;
	}
	if (m_camera_shaking) {
		frame.camera.pos.x += glm::cos(m_camera_modifier * 10) * m_camera_shake_strength;
		frame.camera.pos.y += glm::cos(m_camera_modifier * 15) * m_camera_shake_strength;
		m_camera_shake_timestamp += config.delta_time;
		if (m_camera_shake_timestamp >= m_camera_shake_time) {
			m_camera_shake_timestamp = 0.0f;
			m_camera_shaking = false;
		}
	}

	return true;
}

void ShootingState::mainMenuUpdate(Frame& frame) {

	auto& config = Config::get();

	if (m_state == State::MainMenuStart) {
		frame.world_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_ui->drawSp(frame, m_main_menu_sp_fade);
		if (m_main_menu_sp_displaying == false) {
			if (m_main_menu_sp_fade < 0.001) {
				m_main_menu_sp_fade = 0.0f;
				m_main_menu_sp_displaying = true;
			}
			m_main_menu_sp_fade = lerp(m_main_menu_sp_fade, 0.0f, 0.02);
		}
		if (m_main_menu_sp_displaying == true) {
			if (m_main_menu_sp_display_timestamp >= m_main_menu_sp_display_time) {
				if (m_main_menu_sp_fade > 0.9) {
					m_main_menu_sp_fade = 1.0f;
					m_main_menu_sp_display_timestamp = 0.0f;
					m_main_menu_sp_displaying = false;
					m_main_menu_game_started = false;
					m_main_menu_game_started_delay_timestamp = 0.0f;
					m_wave_count = 0;
					m_cur_lives = m_max_lives;
					m_cur_bullets = m_max_bullets;
					m_state = State::MainMenu;
				}
				m_main_menu_sp_fade = lerp(m_main_menu_sp_fade, 1.0f, 0.01);
			}
			else {
				m_main_menu_sp_display_timestamp += config.delta_time;
			}
		}
	}
	if (m_state == State::MainMenu) {
		frame.world_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (!m_main_menu_game_started) {
			for (const auto& e : frame.input_events) {
				if (e.key == SDL_BUTTON_LEFT && e.state == true) {
					m_main_menu_game_started = true;

					Frame::SoundCommand command;
					command.type = Frame::SoundCommand::Type::PlaySound;
					Frame::SoundCommand::PlaySound play;
					play.handle = m_sound_gunshot;
					play.volume = 0.25f;
					command.data = play;
					frame.sound_commands.push_back(command);

				}
			}
		}
		if (m_main_menu_game_started) {
			if (m_main_menu_game_started_delay_timestamp >= m_main_menu_game_started_delay_time) {
				m_state = State::MainMenuEnd;
			}
			else {
				m_main_menu_game_started_delay_timestamp += config.delta_time;
			}
		}
		m_scenery->drawGround(frame, m_ground_cur_pos + glm::vec2(0.0f, (m_ground_in_cover_pos.y - m_ground_out_cover_pos.y) * m_cover_value));
		m_ui->drawLogo(frame);

	}
	if (m_state == State::MainMenuEnd) {
		m_damaged = true; // Hack

		m_shoot_helper_active = true;
		m_shoot_helper_satisfied = false;
		m_shoot_helper_fade = 0.0f;
		m_shoot_helper_delay_time = 1.0;
		m_shoot_helper_delay_timestamp = 0.0f;

		m_reload_helper_active = true;
		m_reload_helper_satisfied = false;
		m_reload_helper_fade = 0.0f;
		m_reload_helper_delay_time = 1.0;
		m_reload_helper_delay_timestamp = 0.0f;

		m_state = State::MovingStart;
	}

}

void ShootingState::shootingUpdate(Frame& frame) {

	auto& config = Config::get();

	// Parse input
	for (const auto& e : frame.input_events) {
		if (!m_damaged) {
			if (e.key == SDL_BUTTON_RIGHT) {
				if (e.state == true) {
					m_cover_toggled = true;
				}
				else {
					m_cover_toggled = false;
				}
			}
			else if (e.key == SDL_BUTTON_LEFT && e.state == true) {
				if (m_cover_value < m_cover_shoot_requirement) {
					if (m_cur_bullets > 0) {
						m_shoot_helper_satisfied = true;
						WorldCursorPos hitpoint(frame);
						b32 hit = m_ducks->handleHitpoint(hitpoint, m_cover_value);
						Frame::SoundCommand command;
						command.type = Frame::SoundCommand::Type::PlaySound;
						Frame::SoundCommand::PlaySound play;
						play.handle = m_sound_gunshot;
						play.volume = 0.25f;
						command.data = play;
						frame.sound_commands.push_back(command);
						m_cur_bullets -= 1;
						m_camera_shaking = true;
					}
					else {
						Frame::SoundCommand command;
						command.type = Frame::SoundCommand::Type::PlaySound;
						Frame::SoundCommand::PlaySound play;
						play.handle = m_sound_vo_reload;
						play.volume = 1.0f;
						command.data = play;
						frame.sound_commands.push_back(command);
					}
				}
			}
		}
	}

	if (!m_damaged) {
		if (m_cover_toggled) {
			m_cover_value = lerp(m_cover_value, 1.0f, 0.10);
			if (m_cover_value > m_cover_reload_requirement && m_cur_bullets != m_max_bullets) {
				m_reload_helper_satisfied = true;
				Frame::SoundCommand command;
				command.type = Frame::SoundCommand::Type::PlaySound;
				Frame::SoundCommand::PlaySound play;
				play.handle = m_sound_gun_reload;
				play.volume = 1.0f;
				command.data = play;
				frame.sound_commands.push_back(command);
				m_cur_bullets = m_max_bullets;
			}
		}
		else {
			m_cover_value = lerp(m_cover_value, 0.0f, 0.10);
		}
	}
	else {
		m_cover_value = lerp(m_cover_value, 0.0f, 0.10);
	}

	m_ducks->update();

	if (m_shoot_helper_active) {
		if (m_shoot_helper_satisfied) {
			if (m_shoot_helper_delay_timestamp >= m_shoot_helper_delay_time) {
				m_shoot_helper_fade = lerp(m_shoot_helper_fade, 1.0f, 0.10);
			}
			else {
				m_shoot_helper_delay_timestamp += config.delta_time;
			}
			m_ui->drawShootHelper(frame, true, m_shoot_helper_fade);
		}
		else {
			m_ui->drawShootHelper(frame, false, 0.0f);
		}
	}

	if (m_reload_helper_active) {
		if (m_reload_helper_satisfied) {
			if (m_reload_helper_delay_timestamp >= m_reload_helper_delay_time) {
				m_reload_helper_fade = lerp(m_reload_helper_fade, 1.0f, 0.10);
			}
			else {
				m_reload_helper_delay_timestamp += config.delta_time;
			}
			m_ui->drawReloadHelper(frame, true, m_reload_helper_fade);
		}
		else {
			m_ui->drawReloadHelper(frame, false, 0.0f);
		}
	}

	if (m_timer_cur < 3.0f) {
		m_ui->drawLargeTimer(frame, m_timer_cur);
	}

	m_ducks->drawDucks(frame, m_cover_value);
	m_scenery->drawGround(frame, m_ground_cur_pos + glm::vec2(0.0f, (m_ground_in_cover_pos.y - m_ground_out_cover_pos.y) * m_cover_value));
	if (!m_damaged) {
		m_scenery->drawCover(frame, m_cover_cur_pos + glm::vec2(0.0f, (m_cover_in_cover_pos.y - m_cover_out_cover_pos.y) * m_cover_value));
	}
	m_ducks->drawExplosions(frame, m_cover_value);

	m_ui->drawCrosshair(frame);

	m_ui->drawSmallTimer(frame, m_timer_cur);
	if (m_ducks->getLivingDuckCount() > 0) {
		m_timer_cur -= config.delta_time;
		if (m_timer_cur < 0.0f && !m_damaged) {
			Frame::SoundCommand command;
			command.type = Frame::SoundCommand::Type::PlaySound;
			Frame::SoundCommand::PlaySound play;
			play.handle = m_sound_explosion;
			play.volume = 1.0f;
			command.data = play;
			frame.sound_commands.push_back(command);

			m_cur_lives -= 1;
			m_damaged = true;
			m_damaged_timestamp = 0.0f;
			m_ducks->destroyDucks();
			m_camera_shaking = true;
			m_camera_shake_timestamp = 0.0f;
		}
	}

	m_ui->drawBullets(frame, m_cur_bullets);
	m_ui->drawLives(frame, m_cur_lives);

	if (m_damaged) {
		if (m_cur_lives > 0) {
			if (m_damaged_timestamp <= m_damaged_time) {
				m_damaged_timestamp += config.delta_time;
				f32 filter_stegnth = m_damaged_timestamp / m_damaged_time;
				frame.world_color = glm::vec4(
					m_damage_filter_color.x + ((1.0f - m_damage_filter_color.x) * filter_stegnth),
					m_damage_filter_color.y + ((1.0f - m_damage_filter_color.y) * filter_stegnth),
					m_damage_filter_color.z + ((1.0f - m_damage_filter_color.z) * filter_stegnth),
					1.0f);
			}
			else {
				frame.world_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				if (m_cover_value < 0.001) {
					m_wave_count++;
					m_state = State::MovingStart;
				}
			}
		}
		else {
			frame.world_color = m_damage_filter_color;
			m_state = State::GameOverStart;
		}
	}
	else if (m_ducks->getTotalDuckCount() == 0) {
		m_wave_count++;
		m_state = State::MovingStart;
	}

}

void ShootingState::movingUpdate(Frame& frame) {

	auto& config = Config::get();

	if (m_state == State::MovingStart) {
		m_moving_banner_showing = true;
		m_moving_banner_timestamp = 0.0f;
		m_move_timestamp = 0.0f;
		m_move_paralax = 0.0f;
		m_state = State::Moving;
	} 
	if (m_state == State::Moving) {
		frame.world_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_move_timestamp += config.delta_time;
		if (m_move_timestamp < m_move_time) {
			m_move_paralax = lerp(m_move_paralax, 1.0f, 0.10);

			int scaler = 1.0f;
			if (!m_move_direction) {
				scaler = -1.0f;
			}
			m_ground_cur_pos.y = m_ground_out_cover_pos.y + ((m_ground_moving_pos.y - m_ground_out_cover_pos.y) * m_move_paralax);
			m_ground_cur_pos.x += (config.delta_time * m_move_speed) * scaler;
			if (m_ground_cur_pos.x > 15.0f || m_ground_cur_pos.x < -15.0f) {
				m_ground_cur_pos.x = -m_ground_cur_pos.x;
			}
			m_cover_cur_pos.y = m_cover_out_cover_pos.y + ((m_cover_moving_pos.y - m_cover_out_cover_pos.y) * m_move_paralax);

			if (m_shoot_helper_active && m_wave_count > 0) {
				if (m_shoot_helper_satisfied) {
					if (m_shoot_helper_delay_timestamp >= m_shoot_helper_delay_time) {
						m_shoot_helper_fade = lerp(m_shoot_helper_fade, 1.0f, 0.10);
					}
					else {
						m_shoot_helper_delay_timestamp += config.delta_time;
					}
					m_ui->drawShootHelper(frame, true, m_shoot_helper_fade);
				}
				else {
					m_ui->drawShootHelper(frame, false, 0.0f);
				}
			}

			if (m_reload_helper_active && m_wave_count > 0) {
				if (m_reload_helper_satisfied) {
					if (m_reload_helper_delay_timestamp >= m_reload_helper_delay_time) {
						m_reload_helper_fade = lerp(m_reload_helper_fade, 1.0f, 0.10);
					}
					else {
						m_reload_helper_delay_timestamp += config.delta_time;
					}
					m_ui->drawReloadHelper(frame, true, m_reload_helper_fade);
				}
				else {
					m_ui->drawReloadHelper(frame, false, 0.0f);
				}
			}

			m_scenery->drawGround(frame, m_ground_cur_pos);
			if (!m_damaged) {
				m_scenery->drawCover(frame, m_cover_cur_pos);
			}
			m_ui->drawCrosshair(frame);
			m_ui->drawBullets(frame, m_cur_bullets);
			m_ui->drawLives(frame, m_cur_lives);
			m_moving_banner_timestamp += config.delta_time;
			if (m_moving_banner_timestamp >= m_moving_banner_time) {
				m_moving_banner_showing = !m_moving_banner_showing;
				m_moving_banner_timestamp -= m_moving_banner_time;
			}
			if (m_moving_banner_showing) {
				m_ui->drawMovingBanner(frame);
			}

		}
		else {

			m_cur_bullets = m_max_bullets;

			Frame::SoundCommand command;
			command.type = Frame::SoundCommand::Type::PlaySound;
			Frame::SoundCommand::PlaySound play;
			play.handle = m_sound_gun_reload;
			play.volume = 1.0f;
			command.data = play;
			frame.sound_commands.push_back(command);

			m_state = State::MovingEnd;
		}
	}
	if (m_state == State::MovingEnd) {
		m_move_paralax = lerp(m_move_paralax, 0.0f, 0.10);

		m_ground_cur_pos.y = m_ground_out_cover_pos.y + ((m_ground_moving_pos.y - m_ground_out_cover_pos.y) * m_move_paralax);
		m_cover_cur_pos.y = m_cover_out_cover_pos.y + ((m_cover_moving_pos.y - m_cover_out_cover_pos.y) * m_move_paralax);

		if (m_shoot_helper_active && m_wave_count > 0) {
			if (m_shoot_helper_satisfied) {
				if (m_shoot_helper_delay_timestamp >= m_shoot_helper_delay_time) {
					m_shoot_helper_fade = lerp(m_shoot_helper_fade, 1.0f, 0.10);
				}
				else {
					m_shoot_helper_delay_timestamp += config.delta_time;
				}
				m_ui->drawShootHelper(frame, true, m_shoot_helper_fade);
			}
			else {
				m_ui->drawShootHelper(frame, false, 0.0f);
			}
		}

		if (m_reload_helper_active && m_wave_count > 0) {
			if (m_reload_helper_satisfied) {
				if (m_reload_helper_delay_timestamp >= m_reload_helper_delay_time) {
					m_reload_helper_fade = lerp(m_reload_helper_fade, 1.0f, 0.10);
				}
				else {
					m_reload_helper_delay_timestamp += config.delta_time;
				}
				m_ui->drawReloadHelper(frame, true, m_reload_helper_fade);
			}
			else {
				m_ui->drawReloadHelper(frame, false, 0.0f);
			}
		}

		m_scenery->drawGround(frame, m_ground_cur_pos);
		m_scenery->drawCover(frame, m_cover_cur_pos);
		m_ui->drawCrosshair(frame);
		m_ui->drawBullets(frame, m_cur_bullets);
		m_ui->drawLives(frame, m_cur_lives);
		m_ui->drawActionBanner(frame);
		if (m_move_paralax < 0.001) {
			m_move_direction = !m_move_direction;
			m_damaged = false;
			m_timer_cur = m_timer_max;
			m_cover_cur_pos = m_cover_out_cover_pos;
			m_ground_cur_pos.y = m_ground_out_cover_pos.y;
			m_cover_value = 0.0;
			m_cover_toggled = false;
			m_ducks->spawnDucks(m_duck_start_count + ((u32)(m_wave_count / m_duck_count_scaler)), m_duck_start_speed + ((((f32)m_wave_count / 5)) * m_duck_speed_scaler));
			m_state = State::Shooting;
		}
	}

}

void ShootingState::gameOverUpdate(Frame& frame) {

	auto& config = Config::get();

	if (m_state == State::GameOverStart) {

		m_game_over_fade_value = 1.0f;

		m_cover_value = lerp(m_cover_value, 0.0f, 0.10);

		m_ducks->drawDucks(frame, m_cover_value);
		m_scenery->drawGround(frame, m_ground_cur_pos + glm::vec2(0.0f, (m_ground_in_cover_pos.y - m_ground_out_cover_pos.y) * m_cover_value));
		m_ducks->drawExplosions(frame, m_cover_value);

		frame.world_color = m_damage_filter_color;

		m_game_over_freeze_timestamp += config.delta_time;
		if (m_game_over_freeze_timestamp > m_game_over_freeze_time) {
			m_game_over_freeze_timestamp = 0.0f;
			m_state = State::GameOver;
		}

	}
	if (m_state == State::GameOver) {

		m_ducks->update();
		m_ducks->drawDucks(frame, m_cover_value);
		m_scenery->drawGround(frame, m_ground_cur_pos + glm::vec2(0.0f, (m_ground_in_cover_pos.y - m_ground_out_cover_pos.y) * m_cover_value));
		m_ducks->drawExplosions(frame, m_cover_value);
		frame.world_color = m_damage_filter_color;
		m_game_over_post_freeze_timestamp += config.delta_time;
		if (m_game_over_post_freeze_timestamp > m_game_over_post_freeze_time && m_ducks->getTotalDuckCount() == 0) {
			m_game_over_post_freeze_timestamp = 0.0f;
			m_state = State::GameOverEnd;
		}

	}
	if (m_state == State::GameOverEnd) {

		m_scenery->drawGround(frame, m_ground_cur_pos + glm::vec2(0.0f, (m_ground_in_cover_pos.y - m_ground_out_cover_pos.y) * m_cover_value));
		frame.world_color = m_damage_filter_color;
		m_game_over_fade_value = m_game_over_fade_value = lerp(m_game_over_fade_value, 0.0f, 0.01);
		frame.world_color.w =  m_game_over_fade_value;
		m_ui->drawQuacked(frame, m_game_over_fade_value);

		for (const auto& e : frame.input_events) {
			if (e.key == SDL_BUTTON_LEFT && e.state == true) {
				m_moving_banner_showing = true;
				m_moving_banner_timestamp = 0.0f;
				m_move_timestamp = 0.0f;
				m_move_paralax = 0.0f;
				m_state = State::MainMenuStart;
			}
		}

	}

}