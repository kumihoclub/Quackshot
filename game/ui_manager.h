#pragma once

#include "common.h"
#include "cursor.h"
#include "frame.h"

class UIManager {
public:
	UIManager();
	void drawCrosshair(Frame& frame);
	void drawActionBanner(Frame& frame);
	void drawReloadBanner(Frame& frame);
	void drawMovingBanner(Frame& frame);
	void drawHitMarker(Frame& frame);
	void drawSmallTimer(Frame& frame, f32 time);
	void drawLargeTimer(Frame& frame, f32 time);
	void drawLives(Frame& frame, u32 count);
	void drawBullets(Frame& frame, u32 count);
	void drawQuacked(Frame& frame, f32 fade);
	void drawSp(Frame& frame, f32 fade);
	void drawLogo(Frame& frame);
	void drawShootHelper(Frame& frame, b32 green, f32 fade);
	void drawReloadHelper(Frame& frame, b32 green, f32 fade);

private:
	struct {
		u32 number[11];
		u32 bullet = 0;
		u32 health = 0;
		u32 life = 0;
		u32 action_banner = 0;
		u32 reload_banner = 0;
		u32 moving_banner = 0;
		u32 crosshair = 0;
		u32 quacked = 0;
		u32 logo = 0;
		u32 sp = 0;
		u32 helper_shoot = 0;
		u32 helper_reload = 0;
	} m_frames;

	// All of this state is stored here to allow for easy tweaking

	struct {
		glm::vec2 scale = { 1.0f, 1.0f };
		f32 rotation_speed = 120.0f;
		f32 cur_rotation = 0.0f;
	} m_crosshair_state;

	struct {
		glm::vec2 pos = { 0.0f, 0.0f };
		glm::vec2 scale = { 1.0f, 1.0f };
	} m_action_banner_state;

	struct {
		glm::vec2 pos = { 0.0f, 0.0f };
		glm::vec2 scale = { 1.0f, 1.0f };
	} m_reload_banner_state;

	struct {
		glm::vec2 pos = { 0.0f, 0.0f };
		glm::vec2 scale = { 1.0f, 1.0f };
	} m_moving_banner_state;

	struct {
		glm::vec2 scale = { 1.0f, 1.0f };
	} m_hitmarker_state;

	struct {
		glm::vec2 pos = { 0.0f, -7.0f };
		glm::vec2 scale = { 2.0f, 2.0f };
	} m_small_timer_state;

	struct {
		glm::vec2 pos = { 0.0f, 2.0f };
		glm::vec2 scale = { 10.0f, 10.0f };
	} m_large_timer_state;

	struct {
		glm::vec2 pos = { 13.0f, -5.62f };
		glm::vec2 scale = { 1.5f, 1.5f };
		glm::vec2 per_draw_offset = { -1.5f, -0.50f};
	} m_life_state;

	struct {
		glm::vec2 pos = { -13.5f, -6.0f };
		glm::vec2 scale = { 1.5f, 1.5f };
		glm::vec2 per_draw_offset = { 0.90f, -0.11f};
	} m_bullet_state;

	struct {
		glm::vec2 pos = { -0.0f, -0.0f };
		glm::vec2 scale = { 2.0, 2.0 };
	} m_quacked_state;

	struct {
		glm::vec2 pos = { 0.0f, 0.0f };
		glm::vec2 scale = { 1.0, 1.0f };
	} m_sp_state;

	struct {
		glm::vec2 pos = { 0.0f, 1.0f };
		glm::vec2 scale = { 1.0, 1.0 };
	} m_logo_state;
};