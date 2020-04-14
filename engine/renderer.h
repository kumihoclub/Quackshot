#pragma once

#include "common.h"

#include "frame.h"

namespace Renderer {

	void startup();
	void shutdown();
	void draw(Frame& frame);

}