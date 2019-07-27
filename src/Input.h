#pragma once

struct MouseWheel
{
	bool up = false;
	bool down = false;

	// Special boolean for the user only
	bool input_received;
};

struct Input
{
	bool D = false;
	bool F = false;
	bool ESCAPE = false;
	bool LEFT_MOUSEBUTTON = false;
	bool RIGHT_MOUSEBUTTON = false;
};

extern Input currentUpdate;
extern Input lastUpdate;
extern MouseWheel mouseWheel;

void CheckInput(Input* currentUpdate, Input* lastUpdate, MouseWheel* mouseWheel);
