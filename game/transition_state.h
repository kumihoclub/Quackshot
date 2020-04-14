#pragma once

#include "common.h"

#include "scenery_manager.h"
#include "ui_manager.h"

class TransitionState {
public:
	TransitionState() = delete;
	TransitionState(SceneryManager& scenery, UIManager& ui, u32& cur_bullets, u32& cur_lives);
	b32 update(Frame& frame);
private:
	SceneryManager* m_scenery = nullptr;
	UIManager* m_ui = nullptr;
	u32* m_cur_bullets = nullptr;
	u32* m_cur_lives = nullptr;

	glm::vec2 m_ground_moving_pos = glm::vec2(0.0f, -9.0f);
	glm::vec2 m_ground_start_pos = glm::vec2(0.0f, -6.0f);
	f32 m_ground_move_speed = 16.0f;
	glm::vec2 m_cover_moving_pos = glm::vec2(0.0f, -20.0f);
	glm::vec2 m_cover_start_pos = glm::vec2(0.0f, -10.0f);

	f32 m_paralax = 0.0f;

	b32 m_move_direction = true;
	f32 m_move_time = 10.0f;
	f32 m_move_timestamp = 0.0f;

	b32 m_moving_banner_showing = false;
	f32 m_moving_banner_time = 0.5f;
	f32 m_moving_banner_timestamp = 0.0f;

	b32 m_ending = false;
	b32 m_finished = false;
};