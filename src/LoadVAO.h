#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "stb_image.h"
#include "VAO_Data.h"
#include "LibraryTranslations.h"

using namespace glm;

// Everything we need is stored in the scene object - the retrieved data can be used as arguments for the other functions
const aiScene* ImportScene(Assimp::Importer* importer, string path);

vector<DefaultVertex> LoadVertices(aiMesh* mesh);
vector<SkeletalVertex> LoadSkeletalVertices(aiMesh* mesh, vector<vector<mat4>>* boneKeys, const aiScene* scene);
vector<uint> LoadIndices(aiMesh* mesh);

Textures LoadTextures(aiMesh* mesh, const aiScene* scene, string path, Textures texturesLoaded);
Textures LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, string directory, Textures texturesLoaded);

Textures LoadMeshTextures(aiMesh* mesh, const aiScene* scene, string path);
Textures LoadMeshMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, string directory);

uint TextureFromFile(string fileName, string directory);
void DestroyTextures(Textures texturesLoaded);

vector<mat4> ImportGlobalBones(string boneName, uint keyFramesSize, aiNode* root, mat4 offsetMatrix, aiAnimation* animation);

aiMatrix4x4* GetTransform(uint keyFrame, aiNode* root, string boneName, aiMatrix4x4 parentTransform, aiAnimation* animation);

const aiNodeAnim* FindNodeAnim(aiAnimation* pAnimation, string NodeName);

uint FindPosition(uint AnimationTime, const aiNodeAnim* pNodeAnim);
uint FindRotation(uint AnimationTime, const aiNodeAnim* pNodeAnim);
uint FindScaling(uint AnimationTime, const aiNodeAnim* pNodeAnim);
