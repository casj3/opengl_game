#include "transform.h"

#include <glm\gtx\transform.hpp>

glm::mat4 TransformMatrix(Transform transform) {
    glm::mat4 pos_matrix = glm::translate(transform.pos);
    glm::mat4 rot_x_matrix = glm::rotate(transform.rot.x, glm::vec3(1, 0, 0));
    glm::mat4 rot_y_matrix = glm::rotate(transform.rot.y, glm::vec3(0, 1, 0));
    glm::mat4 rot_z_matrix = glm::rotate(transform.rot.z, glm::vec3(0, 0, 1));
    glm::mat4 scale_mat = glm::scale(transform.scale);

    glm::mat4 rot_matrix = rot_z_matrix * rot_y_matrix * rot_x_matrix;

    return pos_matrix * rot_matrix * scale_mat;
}

glm::mat4 GetTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
    glm::mat4 pos_matrix = glm::translate(pos);
    glm::mat4 rot_x_matrix = glm::rotate(rot.x, glm::vec3(1, 0, 0));
    glm::mat4 rot_y_matrix = glm::rotate(rot.y, glm::vec3(0, 1, 0));
    glm::mat4 rot_z_matrix = glm::rotate(rot.z, glm::vec3(0, 0, 1));
    glm::mat4 scale_mat = glm::scale(scale);

    glm::mat4 rot_matrix = rot_z_matrix * rot_y_matrix * rot_x_matrix;

    return pos_matrix * rot_matrix * scale_mat;
}
