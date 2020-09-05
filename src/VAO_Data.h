#pragma once

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <vector>
#include <string>

#include "utils/ArrayManager.h"

// These macros are only valid for a normal map skeletal vertex
#define POSITION_LOCATION 0
#define NORMAL_LOCATION 1
#define TEX_COORD_LOCATION 2
#define BONE_ID_LOCATION 3
#define BONE_WEIGHT_LOCATION 4

#define TANGENT_LOCATION_NMS 3
#define BINORMAL_LOCATION_NMS 4
#define BONE_ID_LOCATION_NMS 5
#define BONE_WEIGHT_LOCATION_NMS 6

struct PhongMaterial
{
    float ka[3];
    float kd[3];
    float ks[3];
    float shininess;
};

// This texture struct should be used in case the texture is imported through ASSIMP. Otherwise
// only an ID is necessary.

struct Texture
{
    uint32_t id;
    std::string type;
    aiString path;
};

struct Textures
{
    std::vector<uint32_t> id;
    std::vector<std::string> types;
    std::vector<aiString> path;
};

struct DefaultVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

struct NormalMapVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 binormal;
};

#define NUM_BONES_PER_VERTEX 4
struct SkeletalVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;

    uint32_t IDs[NUM_BONES_PER_VERTEX];
    float weights[NUM_BONES_PER_VERTEX];
};

void SetupDefaultVAO(uint32_t* VAO, uint32_t* VBO, uint32_t* EBO, std::vector<DefaultVertex> vertices, std::vector<uint32_t> indices);
void SetupSkeletalVAO(uint32_t* VAO, uint32_t* VBO, uint32_t* EBO, std::vector<SkeletalVertex> vertices, std::vector<uint32_t> indices);
void DestroyVAO(uint32_t* VAO);
