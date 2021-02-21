#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "animate.h"
#include "camera.h"

#define MAX_BONES 25

struct Default_Program
{
    uint32_t vertex_shader;
    uint32_t fragment_shader;

    uint32_t program;
};

// As soon there exists more program structures, this union becomes relevant
union Program
{
    Default_Program default_program;
    // to be continued ...
};

struct Default_Uniforms
{
    uint32_t world_matrix;
    uint32_t view_matrix;
    uint32_t perspective_matrix;
};

struct Skinning_Uniforms
{
    uint32_t world_matrix;
    uint32_t view_matrix;
    uint32_t perspective_matrix;
    uint32_t bone_location[MAX_BONES];
};

// A union only takes up space for its most recently assigned variable, which makes it very useful in the superstructure to come
union Uniforms {
    Skinning_Uniforms skinning_uniforms;
    Default_Uniforms default_uniforms;
};

// TODO: Remove the union use everywhere. It's unnecessarily confusing and weird.
// The superstructure is used to group arguments meant for a specific module - in this case the shader/program one
struct Program_Super {
    Uniforms uniforms_type;
    Program program_type;
};

void CreateDefaultProgram(Default_Program* shaders, Default_Uniforms* uniforms);
void CreateSkinningProgram(Default_Program* shaders, Skinning_Uniforms* uniforms);

uint32_t CreateShader(std::string text, uint32_t shaderType);
std::string LoadShader(std::string fileName);
void Bind(uint32_t program);
void DestroyDefaultProgram(Default_Program program);

void UpdateUniforms(glm::mat4 world, glm::mat4 view, glm::mat4 perspective, Default_Uniforms uniforms);
void UpdateSkinningUniforms(glm::mat4 world, Camera camera, Skeleton skeleton, Skinning_Uniforms uniforms);
void UpdateSkinningUniformsOrtho(glm::mat4 world, Ortho_Camera camera, Skeleton skeleton, Skinning_Uniforms uniforms);
void UpdateWorldMatrix(glm::mat4 world, uint32_t world_matrix_uniform);
void UpdateCameraLightBuffer(glm::vec3 lightPos, glm::vec3 cameraPos, uint32_t program);
