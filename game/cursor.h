#pragma once

#include "common.h"
#include "glm/glm.hpp"
#include "frame.h"
#include "config.h"

struct WorldCursorPos {
	WorldCursorPos() = delete;
	WorldCursorPos(Frame& frame);
	const glm::vec2 val();
private:
	glm::vec2 m_pos;
};

struct ScreenCursorPos {
	ScreenCursorPos() = delete;
	ScreenCursorPos(Frame& frame);
	const glm::vec2 val();
private:
	glm::vec2 m_pos;
};

inline WorldCursorPos::WorldCursorPos(Frame& frame) {

	const Config& config = Config::get();
	m_pos = glm::vec2(
		(((f32)frame.mouse_pos_x * (config.viewport_size.x / frame.window_size.x)) / (f32)config.unit_pixels) - (((f32)config.viewport_size.x / (f32)config.unit_pixels) / 2),
		-(((f32)frame.mouse_pos_y * (config.viewport_size.y / frame.window_size.y)) / (f32)config.unit_pixels) - (((f32)config.viewport_size.y / (f32)config.unit_pixels) / -2));
	m_pos -= frame.camera.pos;

}

inline const glm::vec2 WorldCursorPos::val() {
	return m_pos;
}

inline ScreenCursorPos::ScreenCursorPos(Frame& frame) {

	m_pos = glm::vec2(
		(f32)frame.mouse_pos_x / (f32)frame.window_size.x,
		(f32)frame.mouse_pos_y / (f32)frame.window_size.y );
	m_pos -= frame.camera.pos;

}

inline const glm::vec2 ScreenCursorPos::val() {
	return m_pos;
}