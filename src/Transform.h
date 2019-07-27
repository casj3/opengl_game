#pragma once

#include <glm\glm.hpp>

struct Transform
{
  glm::vec3 pos = glm::vec3();
  glm::vec3 rot = glm::vec3();
  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

glm::mat4 TransformMatrix(Transform transform);
