#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <vector>

// These macros are only valid for a normal map skeletal vertex
#define POSITION_LOCATION			0
#define NORMAL_LOCATION				1
#define TEX_COORD_LOCATION			2
#define BONE_ID_LOCATION			3
#define BONE_WEIGHT_LOCATION		4

#define TANGENT_LOCATION_NMS		3
#define BINORMAL_LOCATION_NMS		4
#define BONE_ID_LOCATION_NMS		5
#define BONE_WEIGHT_LOCATION_NMS	6

using namespace glm;
using namespace std;

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
	uint id;
	std::string type;
	aiString path;
};

struct Textures
{
	vector<uint> id;
	vector<std::string> types;
	vector<aiString> path;
};

struct DefaultVertex
{
	vec3 position;
	vec3 normal;
	vec2 tex_coords;
};

struct NormalMapVertex
{
	vec3 position;
	vec3 normal;
	vec2 tex_coords;
	vec3 tangent;
	vec3 binormal;
};

#define NUM_BONES_PER_VERTEX 4
struct SkeletalVertex
{
	vec3 position;
	vec3 normal;
	vec2 tex_coords;

	uint IDs[NUM_BONES_PER_VERTEX];
	float weights[NUM_BONES_PER_VERTEX];
};

void SetupDefaultVAO(uint* VAO, uint* VBO, uint* EBO, vector<DefaultVertex> vertices, vector<uint> indices);
void SetupSkeletalVAO(uint* VAO, uint* VBO, uint* EBO, vector<SkeletalVertex> vertices, vector<uint> indices);
void DestroyVAO(uint* VAO);