#pragma once

#include <glm/glm.hpp>

#include "Input.h"
#include "Camera.h"
#include "Transform.h"

struct RotationIncrementers
{
	float increment_left = 0;
	float increment_right = 0;
};

struct AnimationStateKeyFrames
{
	float state_1;
	float state_2;
};

struct GlideVariables
{
	bool glide_ahead;
	bool glide_again;

	float glide_timer;
};

struct StretchBooleans
{
	bool stretch_out;
	bool pull_in;
};

struct UserSuper
{
	StretchBooleans stretch_booleans;
	GlideVariables glide_variables;
	AnimationStateKeyFrames animation_keys;
	RotationIncrementers rotation_incrementers;
	SlideProperties slide_props;
	Transform user_transform;
	glm::vec3 direction;
	float animation_timer;
};

void IncrementRotation(glm::vec3* direction, glm::vec3* rotation, RotationIncrementers rot, float deltaTime);

void SetCameraPos(glm::vec3* cameraPos, glm::vec3 otherPos);

void Animate(float deltaTime, float* animTimer, AnimationStateKeyFrames animStates, StretchBooleans* stretch, GlideVariables* glide);

void Glide(float deltaTime, GlideVariables* glide, glm::vec3 direction, glm::vec3* pos);

// The calling layer above this function has to check whether mouse wheel input has been received or not to determine if this method should be called or not
void CameraSlide(MouseWheel* wheel, View* view, SlideProperties* slideProperties, float deltaTime);
