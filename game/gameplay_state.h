#pragma once

#include "state.h"
#include "camera.h"
#include "duck_manager.h"
#include "ui_manager.h"
#include "scenery_manager.h"

#include "entt/entt.hpp"

enum class GameplayState {
	MainMenu,
	Shooting,
	Moving,
	Hurt,
	GameOver
};

/*class GameplayState : public State {
public:
	void startup();
	void update(Frame& frame);
	void shutdown();

private:

	entt::registry m_registry;
	DuckManager* m_duck_manager = nullptr;
	UIManager* m_ui_manager = nullptr;
	SceneryManager* m_scenery_manager = nullptr;

	struct {
		u32 current = 10;
		f32 timestamp = 0.0f;
	} m_timer;

	struct {
		u32 max_capacity = 5;
		u32 current_capacity = max_capacity;
		f32 reload_delay = 0.0;
		f32 reload_timestep = 0.0f;
		f32 reload_cover_requirement = 0.0f;
	} m_gun;

	struct {
		u32 max_count = 3;
		u32 current_count = max_count;
	} m_life;

	struct {
		b32 toggled = false;
		f32 percentage = 0.0f;
	} m_cover;

	struct {
		b32 toggled = false;
		f32 paralax_percentage = 0.0f;
		b32 direction = 0.0f;
		f32 cur_move_time = 0.0f;
		f32 target_move_time = 0.0f;
		f32 banner_time = 0.5f;
		f32 banner_timestamp = 0.0f;
		b32 banner_showing = false;
	} m_moving;

	struct {
		b32 active = false;
		f32 time = 2.0f;
		f32 timestamp = 0.0f;
	} m_hurt;

	struct {
		Camera camera;
		f32 shake_strength = 0.25;
		f32 shake_time = 0.0f;
		f32 is_shaking = false;
		u32 cur_shake_frames = 0;
		u32 max_shake_frames = 3;
	} m_camera;

};*/