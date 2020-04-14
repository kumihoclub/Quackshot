#pragma once

#include "common.h"
#include "frame.h"
#include "sprite_cache.h"
#include "camera.h"
#include "cursor.h"

#include "glm/glm.hpp"
#include "entt/entt.hpp"

#include <vector>

class DuckManager {
public:
	struct DuckDying {
		f32 gravity = 0.0f;
		f32 gravity_target = 40.0f;
	};

	struct DuckPath {
		std::vector<glm::vec2> move_nodes;
		glm::vec2 spline_points[4];
		u32 cur_node_index = 0;
		f32 cur_t = 0.0f;
	};

	struct DuckParams {
		glm::vec2 cur_pos = { 0.0f, 0.0f };
		glm::vec2 prev_pos = { 0.0f, 0.0f };
		f32 move_speed = 12.0f;
		f32 anim_speed = 0.1f;
		f32 anim_timestep = 0.0f;
		u32 frame_inc = 0;
		u32 cur_frame = 0;
		b32 direction = true;
	};

	struct Explosion {
		glm::vec2 pos = { 0.0f, 0.0f };
		f32 time = 0.1;
		f32 timestamp = 0.0f;
		u32 frame = 0;
		u32 rot = 0;
	};

	DuckManager();
	~DuckManager();

	void update();
	void drawDucks(Frame& frame, f32 paralax_strength);
	void drawExplosions(Frame& frame, f32 paralax_strength);
	u32 getTotalDuckCount();
	u32 getLivingDuckCount();
	void spawnDucks(u32 count, f32 speed);
	void destroyDucks();
	b32 handleHitpoint(WorldCursorPos& hitpoint, f32 paralax_strength);

	glm::vec2 getDuckPathPointFromT(DuckPath& path, f32 t);
	void addNewDuckPathSplineChainLink(DuckPath& path);
	void generateDuckMathMoveNodes(DuckPath& path);

private:
	entt::registry m_registry;
	u32 m_duck_frames[3];
	u32 m_explosion_frames[3];
	f32 m_paralax_offset = 2.0f;
	u32 m_cur_count = 0;

};