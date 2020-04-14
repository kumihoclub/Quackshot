
#include "ui_manager.h"
#include "sprite_cache.h"

#include <cmath>
#include <iostream>

UIManager::UIManager() {
	for (int i = 0; i < 11; i++) {
		m_frames.number[i] = SpriteCache::fetchSpriteHandle(std::to_string(i)).value();
	}
	m_frames.bullet = SpriteCache::fetchSpriteHandle("bullet").value();
	m_frames.life = SpriteCache::fetchSpriteHandle("life").value();
	m_frames.action_banner = SpriteCache::fetchSpriteHandle("action").value();
	m_frames.reload_banner = SpriteCache::fetchSpriteHandle("reload").value();
	m_frames.moving_banner = SpriteCache::fetchSpriteHandle("moving").value();
	m_frames.crosshair = SpriteCache::fetchSpriteHandle("crosshair").value();
	m_frames.quacked = SpriteCache::fetchSpriteHandle("quacked").value();
	m_frames.sp = SpriteCache::fetchSpriteHandle("sp").value();
	m_frames.logo = SpriteCache::fetchSpriteHandle("logo").value();
	m_frames.helper_shoot = SpriteCache::fetchSpriteHandle("helper_shoot").value();
	m_frames.helper_reload = SpriteCache::fetchSpriteHandle("helper_reload").value();
}

void UIManager::drawCrosshair(Frame& frame) {
	auto& config = Config::get();
	WorldCursorPos cursor_pos(frame);
	Frame::RenderCommand render_command;
	render_command = {};
	render_command.sprite_handle = m_frames.crosshair;
	render_command.scale = m_crosshair_state.scale;
	render_command.pos = cursor_pos.val();
	render_command.rot = m_crosshair_state.cur_rotation;
	frame.screen_render_commands.push_back(render_command);
	m_crosshair_state.cur_rotation += (m_crosshair_state.rotation_speed * config.delta_time);
	if (m_crosshair_state.cur_rotation > 360.0f) {
		m_crosshair_state.cur_rotation -= 360.0f;
	}
}

void UIManager::drawActionBanner(Frame& frame) {
	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_frames.action_banner;
	render_command.scale = m_action_banner_state.scale;
	render_command.pos = m_action_banner_state.pos;
	render_command.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	frame.screen_render_commands.push_back(render_command);
}

void UIManager::drawReloadBanner(Frame& frame) {
	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_frames.reload_banner;
	render_command.scale = m_reload_banner_state.scale;
	render_command.pos = m_reload_banner_state.pos;
	render_command.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	frame.screen_render_commands.push_back(render_command);
}

void UIManager::drawMovingBanner(Frame& frame) {
	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_frames.moving_banner;
	render_command.scale = m_moving_banner_state.scale;
	render_command.pos = m_moving_banner_state.pos;
	render_command.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	frame.screen_render_commands.push_back(render_command);
}

void UIManager::drawHitMarker(Frame& frame) {
	// TODO
}

void UIManager::drawSmallTimer(Frame& frame, f32 time) {
	if (time < 0.0f) time = 0.0f;
	if (time > 10.0f) time = 10.0f;
	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_frames.number[(u32)std::ceil(time)];
	render_command.scale = m_small_timer_state.scale;
	render_command.pos = m_small_timer_state.pos;
	if ((u32)std::ceil(time) < 7 && (u32)std::ceil(time) > 3) {
		render_command.color = { 1.0f, 1.0f, 0.0f, 1.0f };
	}
	else if ((u32)std::ceil(time) < 4) {
		render_command.color = { 1.0f, 0.0f, 0.0f, 1.0f };
	}
	frame.screen_render_commands.push_back(render_command);
}

void UIManager::drawLargeTimer(Frame& frame, f32 time) {
	if (time < 0.0f) time = 0.0f;
	if (time > 10.0f) time = 10.0f;
	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_frames.number[(u32)std::ceil(time)];
	render_command.scale = m_large_timer_state.scale;
	render_command.pos = m_large_timer_state.pos;
	render_command.color = { 1.0f, 0.0f, 0.0f, 0.50f };
	// Big timer is in the background of the world
	frame.world_render_commands.push_back(render_command);
}

void UIManager::drawLives(Frame& frame, u32 count) {
	for (int i = 0; i < count; i++) {
		Frame::RenderCommand render_command;
		render_command.sprite_handle = m_frames.life;
		render_command.scale = m_life_state.scale;
		render_command.pos = m_life_state.pos + (m_life_state.per_draw_offset * (f32)i);
		frame.screen_render_commands.push_back(render_command);
	}
}

void UIManager::drawBullets(Frame& frame, u32 count) {
	for (int i = 0; i < count; i++) {
		Frame::RenderCommand render_command;
		render_command.sprite_handle = m_frames.bullet;
		render_command.scale = m_bullet_state.scale;
		render_command.pos = m_bullet_state.pos + (m_bullet_state.per_draw_offset * (f32)i);
		frame.screen_render_commands.push_back(render_command);
	}
}

void UIManager::drawQuacked(Frame& frame, f32 fade) {
	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_frames.quacked;
	render_command.scale = m_quacked_state.scale;
	render_command.pos = m_quacked_state.pos;
	render_command.color = { 1.0f, 1.0f, 1.0f, (1.0f - fade) };
	frame.screen_render_commands.push_back(render_command);
}

void UIManager::drawSp(Frame& frame, f32 fade) {
	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_frames.sp;
	render_command.scale = m_sp_state.scale;
	render_command.pos = m_sp_state.pos;
	render_command.color = { 1.0f, 1.0f, 1.0f, (1.0 - fade) };
	frame.screen_render_commands.push_back(render_command);
}

void UIManager::drawLogo(Frame& frame) {
	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_frames.logo;
	render_command.scale = m_logo_state.scale;
	render_command.pos = m_logo_state.pos;
	frame.screen_render_commands.push_back(render_command);
}

void UIManager::drawShootHelper(Frame& frame, b32 green, f32 fade) {
	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_frames.helper_shoot;
	render_command.scale = { 1.0f, 1.0f };
	render_command.pos = { 0.0f, 5.0 };
	if (green) {
		render_command.color = { 0.33f, 1.0f, 0.33f, (0.5f - (fade * 0.5f)) };
	}
	else {
		render_command.color = { 1.0f, 1.0f, 1.0f, (0.5f - (fade * 0.5f)) };
	}
	frame.world_render_commands.push_back(render_command);
}

void UIManager::drawReloadHelper(Frame& frame, b32 green, f32 fade) {
	Frame::RenderCommand render_command;
	render_command.sprite_handle = m_frames.helper_reload;
	render_command.scale = { 1.0f, 1.0f };
	render_command.pos = { 0.0f, 2.0f };
	if (green) {
		render_command.color = { 0.33f, 1.0f, 0.33f, (0.5f - (fade * 0.5f)) };
	}
	else {
		render_command.color = { 1.0f, 1.0f, 1.0f, (0.5f - (fade * 0.5f)) };
	}
	frame.world_render_commands.push_back(render_command);
}