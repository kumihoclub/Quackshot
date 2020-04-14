
#include "sim.h"

#include "gameplay_state.h"
#include "shooting_state.h"
#include "scenery_manager.h"
#include "transition_state.h"
#include "duck_manager.h"
#include "ui_manager.h"
#include "sprite_cache.h"
#include "camera.h"

#include <iostream>

namespace {

	GameplayState _cur_state = GameplayState::Shooting;

	SceneryManager* _scenery = nullptr;
	UIManager* _ui = nullptr;


	ShootingState* _shooting = nullptr;
	TransitionState* _transition = nullptr;

	u32 _cur_bullets = 0;
	u32 _cur_lives = 3;

}

void Sim::startup() {
	SDL_ShowCursor(false);

	_scenery = new SceneryManager();
	_ui = new UIManager();

	//_transition = new TransitionState(*_scenery, *_ui, _cur_bullets, _cur_lives);
	//_ducks->spawnDucks(10);
	_shooting = new ShootingState(*_scenery, *_ui);
}

void Sim::shutdown() {

	delete _shooting;
	delete _ui;
	delete _scenery;

	//gameplay_state->shutdown();
	//delete gameplay_state;
	//gameplay_state = nullptr;
}

void Sim::update(Frame& frame) {
	_shooting->update(frame);
	//_transition->update(frame);
	//gameplay_state->update(frame);
}