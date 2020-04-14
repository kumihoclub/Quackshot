
#include "duck_manager.h"
#include "lerp.h"

#include "glm/gtc/random.hpp"
//#include "glm/gtx/vector_angle.hpp"
#define CUTE_C2_IMPLEMENTATION
#include "cute/cute_c2.h"

#include <iostream>

DuckManager::DuckManager() {
	m_duck_frames[0] = SpriteCache::fetchSpriteHandle("duck_0").value();
	m_duck_frames[1] = SpriteCache::fetchSpriteHandle("duck_1").value();
	m_duck_frames[2] = SpriteCache::fetchSpriteHandle("duck_2").value();

	m_explosion_frames[0] = SpriteCache::fetchSpriteHandle("exp_0").value();
	m_explosion_frames[1] = SpriteCache::fetchSpriteHandle("exp_1").value();
	m_explosion_frames[2] = SpriteCache::fetchSpriteHandle("exp_2").value();
	m_explosion_frames[3] = SpriteCache::fetchSpriteHandle("exp_3").value();
	m_explosion_frames[4] = SpriteCache::fetchSpriteHandle("exp_4").value();
}

DuckManager::~DuckManager() {}

void DuckManager::update() {

	auto& config = Config::get();

	auto view = m_registry.view<DuckParams, DuckPath>();
	for (auto entity : view) {
		auto& params = m_registry.get<DuckParams>(entity);
		auto& path = m_registry.get<DuckPath>(entity);
		path.cur_t += params.move_speed * config.delta_time;
		if (path.cur_t >= 1.0f) {
			path.cur_node_index++;
			path.cur_t -= 1.0f;
		}

		if (path.cur_node_index == path.move_nodes.size() - 1) {
			addNewDuckPathSplineChainLink(path);
			generateDuckMathMoveNodes(path);
			path.cur_node_index = 0;
		}

		params.prev_pos = params.cur_pos;
		glm::vec2 lhs = path.move_nodes[path.cur_node_index];
		u32 index_adjusted = path.cur_node_index + 1;
		glm::vec2 rhs = path.move_nodes[index_adjusted];
		params.cur_pos = lerp(lhs, rhs, path.cur_t);
		if (params.cur_pos.x > params.prev_pos.x) {
			params.direction = true;
		}
		else {
			params.direction = false;
		}
		if (params.anim_timestep > params.anim_speed) {
			params.anim_timestep = 0.0f;
			params.frame_inc++;
			if (params.frame_inc > 2) {
				params.frame_inc = 0;
			}
		}
		params.anim_timestep += config.delta_time;
	}

	auto view2 = m_registry.view<DuckParams, DuckDying>();
	for (auto entity : view2) {
		auto& params = m_registry.get<DuckParams>(entity);
		auto& dying = m_registry.get<DuckDying>(entity);
		params.prev_pos = params.cur_pos;
		if (params.direction) {
			params.cur_pos.x += (params.move_speed * config.delta_time);
		}
		else {
			params.cur_pos.x -= (params.move_speed * config.delta_time);
		}
		params.cur_pos.y -= (dying.gravity * config.delta_time);
		if (params.cur_pos.y < -10.0) {
			m_registry.destroy(entity);
			//m_cur_count--;
		}
		if (dying.gravity < dying.gravity_target) {
			dying.gravity += (dying.gravity_target * config.delta_time);			
		}
	}

	auto view3 = m_registry.view<Explosion>();
	for (auto entity : view3) {
		auto& explosion = m_registry.get<Explosion>(entity);
		u32 cur_frame = explosion.frame;
		if (explosion.timestamp >= explosion.time) {
			cur_frame++;
			explosion.frame = cur_frame;
			explosion.timestamp -= explosion.time;
		}
		if (cur_frame < 5) {
			explosion.timestamp += config.delta_time;
		}
		else {
			m_registry.destroy(entity);

		}
	}

}

void DuckManager::drawDucks(Frame& frame, f32 paralax_strength) {

	auto duck_view = m_registry.view<DuckParams>();
	for (auto entity : duck_view) {
		auto& params = m_registry.get<DuckParams>(entity);
		f32 flip_scaler = 1.0f;
		if (!params.direction) {
			flip_scaler *= -1.0f;
		}
		Frame::RenderCommand render_command;
		render_command = {};
		render_command.sprite_handle = m_duck_frames[params.frame_inc];
		render_command.scale = glm::vec2(1.0f * flip_scaler, 1.0f);
		render_command.pos = params.cur_pos + (glm::vec2(0.0f, m_paralax_offset) * paralax_strength);
		if (m_registry.has<DuckDying>(entity)) {
			render_command.rot = glm::atan(params.cur_pos.x - params.prev_pos.x, params.cur_pos.y - params.prev_pos.y) * RADTODEG;
		}
		frame.world_render_commands.push_back(render_command);
	}

}

void DuckManager::drawExplosions(Frame& frame, f32 paralax_strength) {

	auto explosion_view = m_registry.view<Explosion>();
	for (auto entity : explosion_view) {
		auto& explosion = m_registry.get<Explosion>(entity);
		Frame::RenderCommand render_command;
		render_command = {};
		render_command.sprite_handle = m_explosion_frames[explosion.frame];
		render_command.scale = glm::vec2(1.0f, 1.0f) * (f32)(explosion.frame + 1);
		render_command.pos = explosion.pos + (glm::vec2(0.0f, m_paralax_offset) * paralax_strength);
		render_command.rot = explosion.rot;
		render_command.color = { 1.0f, 1.0f, 1.0f, 1.0f / (f32)explosion.frame };
		frame.world_render_commands.push_back(render_command);
	}

}

u32 DuckManager::getTotalDuckCount() {
	// Just throwing the explosion count in here to
	// make sure explosion animations play all the way
	// through, easier than having a check for both
	auto duck_view = m_registry.view<DuckParams>();
	auto explosion_view = m_registry.view<Explosion>();
	return duck_view.size() + explosion_view.size();
}

u32 DuckManager::getLivingDuckCount() {
	auto view = m_registry.view<DuckParams, DuckPath>();
	return view.size();
}

void DuckManager::spawnDucks(u32 count, f32 speed) {

	for (int i = 0; i < count; ++i) {
		u32 new_duck = m_registry.create();
		DuckParams params;
		DuckPath path;
		//path.spline_points[0] = glm::linearRand(glm::vec2(-14.0, -3.0), glm::vec2(14.0, 8.0));
		//path.spline_points[1] = glm::linearRand(glm::vec2(-14.0, -3.0), glm::vec2(14.0, 8.0));
		//path.spline_points[2] = glm::linearRand(glm::vec2(-14.0, -3.0), glm::vec2(14.0, 8.0));
		//path.spline_points[3] = glm::linearRand(glm::vec2(-14.0, -3.0), glm::vec2(14.0, 8.0));
		path.spline_points[0] = glm::linearRand(glm::vec2(-14.0f, -8.0f), glm::vec2(14.0f -8.0f));
		path.spline_points[1] = glm::linearRand(glm::vec2(-14.0, -5.0), glm::vec2(14.0, -5.0));
		path.spline_points[2] = glm::linearRand(glm::vec2(-14.0, -2.0), glm::vec2(14.0, 8.0));
		path.spline_points[3] = glm::linearRand(glm::vec2(-14.0, -2.0), glm::vec2(14.0, 8.0));
		generateDuckMathMoveNodes(path);
		params.anim_timestep = glm::linearRand(0.0f, params.anim_speed);
		params.frame_inc = glm::linearRand(0, 2);
		params.move_speed = speed;
		m_registry.assign<DuckParams>(new_duck, params);
		m_registry.assign<DuckPath>(new_duck, path);
		//m_cur_count++;
	}

}

void DuckManager::destroyDucks() {

	auto view = m_registry.view<DuckParams, DuckPath>();
	for (auto entity : view) {
		m_registry.remove<DuckPath>(entity);
		m_registry.assign<DuckDying>(entity);

		// TODO: Find a better way to do this, EnTT likely has a way to do it in one op
		entt::entity new_explosion = m_registry.create<>();
		m_registry.assign<Explosion>(new_explosion);
		Explosion& ref = m_registry.get<Explosion>(new_explosion);
		ref.pos = m_registry.get<DuckParams>(entity).cur_pos;
		ref.rot = glm::linearRand(0, 360);
	}

}

b32 DuckManager::handleHitpoint(WorldCursorPos& hitpoint, f32 paralax_strength) {

	auto& config = Config::get();

	glm::vec2 adjusted_hitpoint = hitpoint.val() - glm::vec2(0.0f, (m_paralax_offset * paralax_strength));
	c2Circle point;
	point.p = {
		adjusted_hitpoint.x,
		adjusted_hitpoint.y
	};
	point.r = (1.0 / (f32)config.unit_pixels) / 2.0f;
	auto view = m_registry.view<DuckParams, DuckPath>();
	for (auto entity : view) {
		auto& params = m_registry.get<DuckParams>(entity);
		c2AABB aabb;
		aabb.min = { params.cur_pos.x - 1.0f, params.cur_pos.y + -1.0f };
		aabb.max = { params.cur_pos.x + 1.0f, params.cur_pos.y + 1.0f };
		if (c2CircletoAABB(point, aabb)) {
			//m_registry.destroy(entity);
			//DuckPath& path = m_registry.get<DuckPath>(entity);
			m_registry.remove<DuckPath>(entity);
			//DuckDying dying;
			//dying.move_speed = params.prev_pos.x - params.cur_pos.x;
			m_registry.assign<DuckDying>(entity);
			return true;
		}
	}
	return false;
}

glm::vec2 DuckManager::getDuckPathPointFromT(DuckPath& path, f32 t) {
	f32 tt = t * t;
	f32 ttt = tt * t;

	f32 q0 = -ttt + 2.0f * tt - t;
	f32 q1 = 3.0f * ttt - 5.0f * tt + 2.0f;
	f32 q2 = -3.0f * ttt + 4.0f * tt + t;
	f32 q3 = ttt - tt;

	return ((path.spline_points[0] * q0 + path.spline_points[1] * q1 + path.spline_points[2] * q2 + path.spline_points[3] * q3) * 0.5f);
}

void DuckManager::addNewDuckPathSplineChainLink(DuckPath& path) {
	path.spline_points[0] = path.spline_points[1];
	path.spline_points[1] = path.spline_points[2];
	path.spline_points[2] = path.spline_points[3];
	// BRUTE FORCE HACK
	do {
		path.spline_points[3] = glm::linearRand(glm::vec2(-14.0f, -2.0f), glm::vec2(14.0f, 8.0f));
	} while (glm::distance(path.spline_points[2], path.spline_points[3]) < 1.0f);
}

void DuckManager::generateDuckMathMoveNodes(DuckPath& path) {
	f32 length = 0.0f;
	if (path.move_nodes.size() > 0) {
		path.move_nodes[0] = path.move_nodes[path.move_nodes.size() - 1];
		path.move_nodes.resize(1);
	}
	glm::vec2 old_point, new_point;
	old_point = getDuckPathPointFromT(path, 0.0f);
	for (f32 i = 0.005; i < 1.0f; i += 0.005) {
		new_point = getDuckPathPointFromT(path, i);
		length += glm::distance(old_point, new_point);
		old_point = new_point;
		if (length >= 1.0) {
			path.move_nodes.push_back(old_point);
			length = 0.0f;
		}
	}
}