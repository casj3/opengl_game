#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>
//#include <assimp/postprocess.h>
#include "VAO_Data.h"
#include "Animation.h"

// Everything we need is stored in the scene object - the retrieved data can be used as arguments for the other functions
const aiScene* ImportScene(Assimp::Importer* importer, std::string path);

std::vector<DefaultVertex> LoadVertices(aiMesh* mesh);
struct Skeletons LoadSkeletalVertices(aiMesh* mesh, const aiScene* scene);
std::vector<unsigned int> LoadIndices(aiMesh* mesh);

Textures LoadTextures(aiMesh* mesh, const aiScene* scene, std::string path, Textures texturesLoaded);
Textures LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory, Textures texturesLoaded);

Textures LoadMeshTextures(aiMesh* mesh, const aiScene* scene, std::string path);
Textures LoadMeshMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory);

unsigned int TextureFromFile(std::string fileName, std::string directory);
void DestroyTextures(Textures texturesLoaded);

std::vector<glm::mat4> ImportGlobalBones(std::string boneName, unsigned int keyFramesSize, aiNode* root, glm::mat4 offsetMatrix, aiAnimation* animation);

aiMatrix4x4* GetTransform(unsigned int keyFrame, aiNode* root, std::string boneName, aiMatrix4x4 parentTransform, aiAnimation* animation);

const aiNodeAnim* FindNodeAnim(aiAnimation* pAnimation, std::string NodeName);

unsigned int FindPosition(unsigned int AnimationTime, const aiNodeAnim* pNodeAnim);
unsigned int FindRotation(unsigned int AnimationTime, const aiNodeAnim* pNodeAnim);
unsigned int FindScaling(unsigned int AnimationTime, const aiNodeAnim* pNodeAnim);
