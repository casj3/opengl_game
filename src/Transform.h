#pragma once

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

using namespace glm;

struct Transform
{
	vec3 pos = vec3();
	vec3 rot = vec3();
	vec3 scale = vec3(1.0f, 1.0f, 1.0f);
};

mat4 TransformMatrix(Transform transform);