#pragma once

#include <stdint.h>

#include "gpu_store.h"

// OBS! This is being reconfigured for the sake of debugging
// 2019-07-27:
// I don't remember what I meant by that.
struct Draw_Units_Textured {
    std::vector<Textures> textures;
    std::vector<uint32_t> vertex_array_objects;
    std::vector<uint32_t> indices_size;
};

// Maybe this structure should use many different types of materials. It is possible.
// Don't know what the use of it would be though.
// 2019-07-27:
// SoA?
struct Draw_Units_Phong
{
    std::vector<Phong_Material> phong_materials;
    std::vector<uint32_t> vertex_array_objects;
    std::vector<uint32_t> indices_size;
};

void DrawTexturedPhongVAO(uint32_t program, uint32_t VAO, Textures textures, uint32_t indicesSize, Phong_Material material);
void DrawTexturedVAO(uint32_t program, uint32_t VAO, Textures textures, uint32_t indicesSize);
void DrawPhongVAO(uint32_t program, uint32_t VAO, uint32_t indicesSize, Phong_Material material);
