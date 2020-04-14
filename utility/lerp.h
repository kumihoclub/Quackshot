#pragma once

#include "common.h"
#include "glm/glm.hpp"

inline glm::vec2 lerp(glm::vec2 lhs, glm::vec2 rhs, f32 t) {
	return lhs * (1.f - t) + rhs * t;
}

inline f32 lerp(f32 lhs, f32 rhs, f32 t) {
	return lhs * (1.f - t) + rhs * t;
}