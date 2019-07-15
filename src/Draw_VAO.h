#pragma once

#include "VAO_Data.h"
#include <sstream>

// OBS! This is being reconfigured for the sake of debugging
struct DrawUnitsTextured
{
	vector<Textures> textures;
	vector<uint> vertex_array_objects;
	vector<uint> indices_size;
};

// Maybe this structure should use many different types of materials. It is possible. Don't know what the use of it would be though.
struct DrawUnitsPhong
{
	vector<PhongMaterial> phong_materials;
	vector<uint> vertex_array_objects;
	vector<uint> indices_size;
};

void DrawTexturedPhongVAO(uint program, uint VAO, Textures textures, uint indicesSize, PhongMaterial material);
void DrawTexturedVAO(uint program, uint VAO, Textures textures, uint indicesSize);
void DrawPhongVAO(uint program, uint VAO, uint indicesSize, PhongMaterial material);
