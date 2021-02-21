#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "animate.h"
#include "utils/array_pool.h"
#include "gpu_store.h"

// Everything we need is stored in the scene object - the retrieved data can be used as arguments for the other functions
const aiScene* ImportScene(Assimp::Importer* importer, std::string path);

std::vector<Default_Vertex> LoadVertices(aiMesh* mesh);

/// Loads a skeleton from a scene. Assumes a scene to skeleton correspondence, i.e. one skeleton per scene.
///
/// @param[in] scene  Holds the skeleton data to load.
/// @param[out] outVerticies  Is assigned the handle to the skeleton vertex data.
///
/// @returns the skeleton.
Skeleton LoadSkeleton(const aiScene* scene, ARRAY_HANDLE(Skeletal_Vertex)* outVertices);
std::vector<uint32_t> LoadIndices(aiMesh* mesh);

Textures LoadTextures(aiMesh* mesh, const aiScene* scene, std::string path, Textures texturesLoaded);
Textures LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory, Textures texturesLoaded);

Textures LoadMeshTextures(aiMesh* mesh, const aiScene* scene, std::string path);
Textures LoadMeshMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory);

uint32_t TextureFromFile(std::string fileName, std::string directory);
void DestroyTextures(Textures texturesLoaded);
