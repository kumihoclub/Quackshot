#pragma once

#include "common.h"

#include "frame.h"
#include "scenery_manager.h"
#include "ui_manager.h"
#include "duck_manager.h"

class ShootingState {
public:
	enum class State {
		MainMenuStart,
		MainMenu,
		MainMenuEnd,
		Shooting,
		MovingStart,
		Moving,
		MovingEnd,
		GameOverStart,
		GameOver,
		GameOverEnd
	};
	ShootingState() = delete;
	ShootingState(SceneryManager& scenery, UIManager& ui);
	~ShootingState();
	b32 update(Frame& frame);
private:
	SceneryManager* m_scenery = nullptr;
	DuckManager* m_ducks = nullptr;
	UIManager* m_ui = nullptr;

	void mainMenuUpdate(Frame& frame);
	void shootingUpdate(Frame& frame);
	void movingUpdate(Frame& frame);
	void gameOverUpdate(Frame& frame);
	State m_state = State::MainMenuStart;

	b32 m_damaged = false;
	f32 m_damaged_time = 2.0;
	f32 m_damaged_timestamp = 0.0f;
	glm::vec4 m_damage_filter_color = { 1.0f, 0.0f, 0.0f, 1.0f };

	u32 m_max_bullets = 5;
	u32 m_max_lives = 3;
	u32 m_cur_bullets = m_max_bullets;
	u32 m_cur_lives = m_max_lives;

	f32 m_duck_start_speed = 8.0f;
	u32 m_duck_start_count = 3;
	f32 m_duck_speed_scaler = 1.0f;
	u32 m_duck_count_scaler = 3;

	u32 m_wave_count = 0;

	b32 m_cover_toggled = false;
	f32 m_cover_value = 0.0f;
	f32 m_cover_reload_requirement = 0.66;
	f32 m_cover_shoot_requirement = 0.33;

	glm::vec2 m_ground_in_cover_pos = glm::vec2(0.0f, -1.0f);
	glm::vec2 m_ground_out_cover_pos = glm::vec2(0.0f, -6.0f);
	glm::vec2 m_ground_moving_pos = glm::vec2(0.0f, -9.0f);
	glm::vec2 m_ground_cur_pos = m_ground_out_cover_pos;

	glm::vec2 m_cover_moving_pos = glm::vec2(0.0f, -20.0f);
	glm::vec2 m_cover_in_cover_pos = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_cover_out_cover_pos = glm::vec2(0.0f, -10.0f);
	glm::vec2 m_cover_cur_pos = glm::vec2(0.0f, 0.0f);

	f32 m_timer_max = 10.0f;
	f32 m_timer_cur = m_timer_max;

	f32 m_camera_shaking = false;
	f32 m_camera_shake_strength = 0.25;
	f32 m_camera_shake_time = 0.05;
	f32 m_camera_shake_timestamp = 0.0f;
	f32 m_camera_modifier = 1.0f;

	f32 m_move_paralax = 0.0f;
	f32 m_move_speed = 16.0f;
	b32 m_move_direction = true;
	f32 m_move_time = 3.0;
	f32 m_move_timestamp = 0.0f;

	b32 m_moving_banner_showing = false;
	f32 m_moving_banner_time = 0.5f;
	f32 m_moving_banner_timestamp = 0.0f;

	f32 m_game_over_freeze_time = 0.5;
	f32 m_game_over_freeze_timestamp = 0.0f;
	f32 m_game_over_post_freeze_time = 1.0;
	f32 m_game_over_post_freeze_timestamp = 0.0f;
	f32 m_game_over_fade_value = 1.0f;

	u32 m_sound_hit = 0;
	u32 m_sound_vo_reload = 0;
	u32 m_sound_gun_reload = 0;
	u32 m_sound_gunshot = 0;
	u32 m_sound_explosion = 0;

	f32 m_main_menu_sp_fade = 1.0f;
	f32 m_main_menu_sp_display_time = 1.0f;
	f32 m_main_menu_sp_display_timestamp = 0.0f;
	b32 m_main_menu_sp_displaying = false;
	b32 m_main_menu_game_started = false;
	f32 m_main_menu_game_started_delay_time = 2.0;
	f32 m_main_menu_game_started_delay_timestamp = 0.0f;

	b32 m_shoot_helper_active = true;
	b32 m_shoot_helper_satisfied = false;
	f32 m_shoot_helper_fade = 0.0f;
	f32 m_shoot_helper_delay_time = 1.0;
	f32 m_shoot_helper_delay_timestamp = 0.0f;

	b32 m_reload_helper_active = true;
	b32 m_reload_helper_satisfied = false;
	f32 m_reload_helper_fade = 0.0f;
	f32 m_reload_helper_delay_time = 1.0;
	f32 m_reload_helper_delay_timestamp = 0.0f;

};