#pragma once

#include "common.h"
#include "frame.h"

#include "state.h"

namespace Sim {
	void startup();
	void update(Frame& frame);
	void shutdown();
}