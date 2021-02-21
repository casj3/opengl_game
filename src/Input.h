#pragma once

struct Mouse_Wheel
{
    bool up = false;
    bool down = false;

    // Special boolean for the user only
    bool input_received;
};

struct Input
{
    bool d = false;
    bool f = false;
    bool esc = false;
    bool left_mouse_btn = false;
    bool right_mouse_btn = false;
};

// TODO: Make these static, move them to the source file and add functions for retrieving const copies of them.
extern Input current_update;
extern Input last_update;
extern Mouse_Wheel mouse_wheel;

void CheckInput(Input* currentUpdate, Input* lastUpdate, Mouse_Wheel* mouseWheel);
