#pragma once

#include "common.h"

#include "frame.h"

class State {
public:
	virtual void startup() = 0;
	virtual void update(Frame& frame) = 0;
	virtual void shutdown() = 0;
};