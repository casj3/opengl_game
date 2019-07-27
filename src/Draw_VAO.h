#pragma once

#include "VAO_Data.h"

// OBS! This is being reconfigured for the sake of debugging
// 2019-07-27:
// I don't remember what I meant by that.
struct DrawUnitsTextured
{
	std::vector<Textures> textures;
	std::vector<unsigned int> vertex_array_objects;
	std::vector<unsigned int> indices_size;
};

// Maybe this structure should use many different types of materials. It is possible.
// Don't know what the use of it would be though.
// 2019-07-27:
// SoA?
struct DrawUnitsPhong
{
	std::vector<PhongMaterial> phong_materials;
	std::vector<unsigned int> vertex_array_objects;
	std::vector<unsigned int> indices_size;
};

void DrawTexturedPhongVAO(unsigned int program, unsigned int VAO, Textures textures, unsigned int indicesSize, PhongMaterial material);
void DrawTexturedVAO(unsigned int program, unsigned int VAO, Textures textures, unsigned int indicesSize);
void DrawPhongVAO(unsigned int program, unsigned int VAO, unsigned int indicesSize, PhongMaterial material);
