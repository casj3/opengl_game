#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Animation.h"
#include "Camera.h"

#define MAX_BONES 25

struct DefaultProgram
{
	unsigned int vertex_shader;
	unsigned int fragment_shader;

	unsigned int program;
};

// As soon there exists more program structures, this union becomes relevant
union Program
{
	DefaultProgram defaultProgram;
	// to be continued ...
};

struct ShaderProgramFiles
{
	std::string vertex_shader_file;
	std::string fragment_shader_file;
};

struct DefaultUniforms
{
	unsigned int world_matrix;
	unsigned int view_matrix;
	unsigned int perspective_matrix;
};

struct SkinningUniforms
{
	unsigned int world_matrix;
	unsigned int view_matrix;
	unsigned int perspective_matrix;
	unsigned int bone_location[MAX_BONES];
};

// A union only takes up space for its most recently assigned variable, which makes it very useful in the superstructure to come
union Uniforms
{
	SkinningUniforms skinningUniforms;
	DefaultUniforms defaultUniforms;
};

// The superstructure is used to group arguments meant for a specific module - in this case the shader/program one
struct ProgramSuper
{
	Uniforms uniforms_type;
	Program program_type;
};

void CreateDefaultProgram(DefaultProgram* shaders, DefaultUniforms* uniforms);
void CreateSkinningProgram(DefaultProgram* shaders, SkinningUniforms* uniforms);

unsigned int CreateShader(std::string text, unsigned int shaderType);
std::string LoadShader(std::string fileName);
void Bind(unsigned int program);
void DestroyDefaultProgram(DefaultProgram program);

void UpdateUniforms(glm::mat4 world, glm::mat4 view, glm::mat4 perspective, DefaultUniforms uniforms);
void UpdateSkinningUniforms(glm::mat4 world, Camera camera, SkeletonSuper skeleton, SkinningUniforms uniforms);
void UpdateSkinningUniformsOrtho(glm::mat4 world, OrthoCamera camera, SkeletonSuper skeleton, SkinningUniforms uniforms);
void UpdateWorldMatrix(glm::mat4 world, unsigned int world_matrix_uniform);
void UpdateCameraLightBuffer(glm::vec3 lightPos, glm::vec3 cameraPos, unsigned int program);
