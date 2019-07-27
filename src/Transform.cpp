#include "Transform.h"

#include <glm\gtx\transform.hpp>

glm::mat4 TransformMatrix(Transform transform)
{
  glm::mat4 posMatrix = glm::translate(transform.pos);
	glm::mat4 rotXMatrix = glm::rotate(transform.rot.x, glm::vec3(1, 0, 0));
	glm::mat4 rotYMatrix = glm::rotate(transform.rot.y, glm::vec3(0, 1, 0));
	glm::mat4 rotZMatrix = glm::rotate(transform.rot.z, glm::vec3(0, 0, 1));
	glm::mat4 scaleMatrix = glm::scale(transform.scale);

  glm::mat4 rotMatrix = rotZMatrix * rotYMatrix * rotXMatrix;

	return posMatrix * rotMatrix * scaleMatrix;
}
