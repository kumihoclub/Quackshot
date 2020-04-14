
#include "scenery_manager.h"

#include "sprite_cache.h"

SceneryManager::SceneryManager() {

	m_ground_frame = SpriteCache::fetchSpriteHandle("ground").value();
	m_cover_frame = SpriteCache::fetchSpriteHandle("log").value();

}

/*void SceneryManager::flagMoving(b32 val) {
	m_moving = val;
}

void SceneryManager::moveGround(f32 val, b32 left_right) {

	int scaler = 1.0f;
	if (!left_right) {
		scaler = -1.0f;
	}
	m_ground.cur_pos.x += (val * m_ground.move_speed) * scaler;
	if (m_ground.cur_pos.x > 15.0f || m_ground.cur_pos.x < -15.0f) {
		m_ground.cur_pos.x = -m_ground.cur_pos.x;
	}

}*/

void SceneryManager::drawGround(Frame& frame, glm::vec2 pos) {

	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_ground_frame;
	render_command.scale = { 2.0f, 2.0f };
	render_command.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	render_command.pos = pos;
	frame.world_render_commands.push_back(render_command);

	if (render_command.pos.x != 0.0f) {

		f32 scaler = 1.0f;
		if (render_command.pos.x > 0.0f) scaler = -1.0f;

		render_command.sprite_handle = m_ground_frame;
		render_command.scale = { 2.0f, 2.0f };
		render_command.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		render_command.pos = glm::vec2(pos.x + (30.0f * scaler), pos.y);
		frame.world_render_commands.push_back(render_command);
	}

}
void SceneryManager::drawCover(Frame& frame, glm::vec2 pos) {

	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_cover_frame;
	render_command.scale = { 7.5f, 7.5f };
	render_command.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	render_command.pos = pos;
	frame.world_render_commands.push_back(render_command);

}