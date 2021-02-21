#pragma once

#include <stdint.h>

#include <glm/glm.hpp>
#include "gpu_store.h"
#include "utils/math_functions.h"

struct Quad
{
    Default_Vertex v0, v1, v2, v3;

    uint32_t indices[6];
};

struct Polygon
{
    Default_Vertex v0, v1, v2;

    uint32_t indices[3];
};

Quad CreateDefaultQuad()
{
    Quad quad;

    quad.v0.position = glm::vec3(0, 0, 0);
    quad.v0.tex_coords = glm::vec2(0, 0);

    quad.v1.position = glm::vec3(0, 1, 0);
    quad.v1.tex_coords = glm::vec2(0, 1);

    quad.v2.position = glm::vec3(1, 1, 0);
    quad.v2.tex_coords = glm::vec2(1, 1);

    quad.v3.position = glm::vec3(1, 0, 0);
    quad.v3.tex_coords = glm::vec2(1, 0);

    ComputeNormalsQuad(&quad.v0, &quad.v1, &quad.v2, &quad.v3);

    quad.indices[0] = 0; quad.indices[1] = 1; quad.indices[2] = 2;
    quad.indices[3] = 2; quad.indices[4] = 3; quad.indices[5] = 0;

    return quad;
}

Polygon CreateDefaultPolygon()
{
    Polygon polygon;

    polygon.v0.position = glm::vec3(0, 0, 0);
    polygon.v0.tex_coords = glm::vec2(0, 0);

    polygon.v1.position = glm::vec3(0.5f, 1, 0);
    polygon.v1.tex_coords = glm::vec2(0.5f, 1);

    polygon.v2.position = glm::vec3(1, 0, 0);
    polygon.v2.tex_coords = glm::vec2(1, 0);

    ComputeNormalsPolygon(&polygon.v0, &polygon.v1, &polygon.v2);

    polygon.indices[0] = 0; polygon.indices[1] = 1; polygon.indices[2] = 2;

    return polygon;
}
