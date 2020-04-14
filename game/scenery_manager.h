#pragma once

#include "common.h"
#include "frame.h"

class SceneryManager {
public:
	SceneryManager();
	//void flagMoving(b32 val);
	//void moveGround(f32 val, b32 left_right);
	void drawGround(Frame& frame, glm::vec2 pos);
	void drawCover(Frame& frame, glm::vec2 pos);

private:
	//b32 m_moving = false;
	u32 m_ground_frame = 0;
	u32 m_cover_frame = 0;


	/*struct {
		glm::vec2 start_pos = glm::vec2(0.0f, -6.0f);
		glm::vec2 cur_pos = start_pos;
		f32 cover_paralax = 5.0f;
		f32 moving_paralax = -3.0;
		u32 frame = 0;
		f32 move_speed = 16.0;
	} m_ground;

	struct {
		glm::vec2 pos = glm::vec2(0.0f, -10.0f);
		u32 frame = 0;
		f32 cover_paralax = 11.0f;
		f32 moving_paralax = -10.0; // just to get it completly out of the way
	} m_cover;*/

};