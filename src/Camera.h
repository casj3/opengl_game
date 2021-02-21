#pragma once

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

#include "window.h"

struct Projection
{
    float fov = 55.0f;
    float aspect = WIN_WIDTH / WIN_HEIGHT;
    float z_near = 0.01f;
    float z_far = 1000;
};

struct Ortho_Projection
{
    float z_near = 1;
    float z_far = 1000;
};

struct View
{
    glm::vec3 pos = glm::vec3(0, 0, 0);
    glm::vec3 forward = glm::vec3(0, 0, 1);
    glm::vec3 up = glm::vec3(0, 1, 0);
};

struct Camera
{
    Projection projection;
    View view;
};

struct Ortho_Camera
{
    Ortho_Projection projection;
    View view;
};

struct Zoom_Tracker
{
    float dolly;
    float zoom;
};

struct Slide_Properties
{
    float slide_timer;
    float deacceleration;
    float acceleration;
    float track_value;

    bool slide;
};

glm::mat4 ViewMatrix(View view);

glm::mat4 ProjectionMatrix(Projection projection);

glm::mat4 OrthoProjectionMatrix(Ortho_Projection projection);

// The resulting float is meant to be added to the z component of the view position instanced at a higher level
float Track(float trackValue, float* slideTimer, float deltaTime);

// This function only works in co-operation with Track - both use the same slideTimer instance
float Slide(Slide_Properties* slideProperties, float deltaTime);
