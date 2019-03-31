#pragma once

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

using namespace glm;

#define WIDTH 1280
#define HEIGHT 720

//#define WIDTH 720
//#define HEIGHT 1280

struct Projection
{
	float fov = 55.0f;
	float aspect = (float)WIDTH / HEIGHT;
	float z_near = 0.01f;
	float z_far = 1000;
};

struct OrthoProjection
{
	float z_near = 0.01f;
	float z_far = 1000;
};

struct View
{
	vec3 pos = vec3(0, 0, -8);
	vec3 forward = vec3(0, 0, 1);
	vec3 up = vec3(0, 1, 0);
};

struct Camera
{
	Projection projection;
	View view;
};

struct OrthoCamera
{
	OrthoProjection projection;
	View view;
};

struct ZoomTracker
{
	float dolly;
	float zoom;
};

struct SlideProperties
{
	float slide_timer;
	float deacceleration;
	float acceleration;
	float track_value;

	bool slide;
};

mat4 ViewMatrix(View view);

mat4 ProjectionMatrix(Projection projection);

mat4 OrthoProjectionMatrix(OrthoProjection projection);

// The resulting float is meant to be added to the z component of the view position instanced at a higher level
float Track(float trackValue, float* slideTimer, float deltaTime);

// This function only works in co-operation with Track - both use the same slideTimer instance
float Slide(SlideProperties* slideProperties, float deltaTime);
