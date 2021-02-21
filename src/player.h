#pragma once

#include <glm/glm.hpp>

#include "input.h"
#include "camera.h"
#include "transform.h"

struct Rotation_Incrementers
{
    float increment_left = 0;
    float increment_right = 0;
};

struct Animation_State_Key_Frames
{
    float state_1;
    float state_2;
};

struct Glide_Variables
{
    bool glide_ahead;
    bool glide_again;

    float glide_timer;
};

struct Stretch_Booleans
{
    bool stretch_out;
    bool pull_in;
};

struct User_Super
{
    Stretch_Booleans stretch_booleans;
    Glide_Variables glide_variables;
    Animation_State_Key_Frames animation_keys;
    Rotation_Incrementers rotation_incrementers;
    Slide_Properties slide_props;
    Transform user_transform;
    glm::vec3 direction;
    float animation_timer;
};

void IncrementRotation(glm::vec3* direction, glm::vec3* rotation, Rotation_Incrementers rot, float deltaTime);

void SetCameraPos(glm::vec3* cameraPos, glm::vec3 otherPos);

void Animate(float deltaTime, float* animTimer, Animation_State_Key_Frames animStates, Stretch_Booleans* stretch, Glide_Variables* glide);

void Glide(float deltaTime, Glide_Variables* glide, glm::vec3 direction, glm::vec3* pos);

// The calling layer above this function has to check whether mouse wheel input has been received or not to determine if this method should be called or not
void CameraSlide(Mouse_Wheel* wheel, View* view, Slide_Properties* slideProperties, float deltaTime);
