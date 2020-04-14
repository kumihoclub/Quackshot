
#include "transition_state.h"

#include "lerp.h"

TransitionState::TransitionState(SceneryManager& scenery, UIManager& ui, u32& cur_bullets, u32& cur_lives) {
	m_scenery = &scenery;
	m_ui = &ui;
	m_cur_bullets = &cur_bullets;
	m_cur_lives = &cur_lives;

}

b32 TransitionState::update(Frame& frame) {

	const Config& config = Config::get();

	if (!m_ending) {
		m_move_timestamp += config.delta_time;
		if (m_move_timestamp < m_move_time) {
			m_paralax = lerp(m_paralax, 1.0f, 0.10);
			//m_scenery_manager->moveGround(config.delta_time, m_moving.direction);

			int scaler = 1.0f;
			if (!m_move_direction) {
				scaler = -1.0f;
				m_move_direction = !m_move_direction;
			}
			glm::vec2 ground_pos = m_ground_start_pos + ((m_ground_moving_pos - m_ground_start_pos) * m_paralax);
			ground_pos.x += (config.delta_time * m_ground_move_speed) * scaler;
			if (ground_pos.x > 15.0f || ground_pos.x < -15.0f) {
				ground_pos.x = -ground_pos.x;
			}


			m_scenery->drawGround(frame, ground_pos);
			//m_scenery_manager->drawCover(frame, m_moving.paralax_percentage);
			m_ui->drawCrosshair(frame);
			m_moving_banner_timestamp += config.delta_time;
			if (m_moving_banner_timestamp >= m_moving_banner_time) {
				m_moving_banner_showing = !m_moving_banner_showing;
				m_moving_banner_timestamp -= m_moving_banner_time;
			}
			if (m_moving_banner_showing) {
				m_ui->drawMovingBanner(frame);
			}
		}
		/*else {
			m_moving.paralax_percentage = lerp(m_moving.paralax_percentage, 0.0f, 0.10);
			m_scenery_manager->drawGround(frame, m_moving.paralax_percentage);
			m_scenery_manager->drawCover(frame, m_moving.paralax_percentage);
			m_ui_manager->drawCrosshair(frame);
			m_ui_manager->drawActionBanner(frame);
			if (m_moving.paralax_percentage <= 0.001f) {
				m_moving.toggled = false;
				m_cover.percentage = 0.0f;
				m_timer = {};
				m_scenery_manager->flagMoving(false);
				m_duck_manager->spawnDucks(10);
				m_gun.current_capacity = m_gun.max_capacity;
			}
		}*/
		else {

		}
	}

	return m_finished;

}