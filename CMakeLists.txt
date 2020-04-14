cmake_minimum_required(VERSION 3.6)
project (duck_crisis)

set(GRIMOIRE_SRCS
	engine/src/graphics.cpp
	engine/src/frame.cpp
	engine/src/config.cpp
	engine/src/platform.cpp
	engine/src/graphics_context.cpp
	)


set(DUCK_CRISIS_SRCS
	#game/src/gameplay_state.cpp
	game/src/sim.cpp
	engine/src/sprite_cache.cpp
	engine/src/renderer.cpp
	game/src/duck_manager.cpp
	game/src/ui_manager.cpp
	game/src/scenery_manager.cpp
	engine/src/sound_engine.cpp
	game/src/shooting_state.cpp
	game/src/transition_state.cpp
	) 

add_executable(duck_crisis WIN32 main.cpp ${GRIMOIRE_SRCS} ${DUCK_CRISIS_SRCS})

set_property(TARGET duck_crisis PROPERTY CXX_STANDARD 17)
set_property(TARGET duck_crisis PROPERTY CXX_STANDARD_REQUIRED ON)

#set_target_properties(duck_crisis PROPERTIES
#LINK_FLAGS /SUBSYSTEM:WINDOWS
#)

target_link_libraries(duck_crisis
	opengl32.lib
	${CMAKE_CURRENT_SOURCE_DIR}/dep/SDL2/lib/SDL2.lib
	${CMAKE_CURRENT_SOURCE_DIR}/dep/SDL2/lib/SDL2main.lib
	${CMAKE_CURRENT_SOURCE_DIR}/dep/SDL_mixer/lib/SDL2_mixer.lib
	)

include_directories(
	dep
	engine
	utility
	game
	)