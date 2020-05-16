#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "Animation.h"
#include <utils/Allocator.h>
#include "VAO_Data.h"

template <typename K, typename V>
struct Pair {
    K key;
    V value;
};

/// Holds the global values for a node given by its ID, for all key-frames.
struct NodeKeyFrameValues {
    const char* node_id;
    AppendArray<Pair<uint32_t, glm::vec3>> positions;
    AppendArray<Pair<uint32_t, glm::vec3>> rotations; // euler angles
    AppendArray<Pair<uint32_t, glm::vec3>> scalings;
};

// Everything we need is stored in the scene object - the retrieved data can be used as arguments for the other functions
const aiScene* ImportScene(Assimp::Importer* importer, std::string path);

std::vector<DefaultVertex> LoadVertices(aiMesh* mesh);
ArrayHandle<SkeletalVertex> LoadSkeletalVertices(const aiScene* scene, uint32_t* skeleton_id);
std::vector<unsigned int> LoadIndices(aiMesh* mesh);

Textures LoadTextures(aiMesh* mesh, const aiScene* scene, std::string path, Textures texturesLoaded);
Textures LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory, Textures texturesLoaded);

Textures LoadMeshTextures(aiMesh* mesh, const aiScene* scene, std::string path);
Textures LoadMeshMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory);

unsigned int TextureFromFile(std::string fileName, std::string directory);
void DestroyTextures(Textures texturesLoaded);
