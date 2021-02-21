#include "load.h"

#include <cstring>

#include <GL/glew.h>
#include <glm/gtx/quaternion.hpp>
#include <assimp/postprocess.h>
#include "utils/lib_convert.h"
#include "transform.h"
#include "utils/print.h"
#include "utils/pair.h"

// TODO: Use a newer version of stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define FIRST_KEY_FRAME  0

struct Ai_Bone_Res {
    aiMesh* mesh;
    aiAnimation* animation;
};

/// Holds the global transform values for a node given by its ID, for all key-frames.
struct Node_Anim_Data {
    const char* node_id;
    T_Append_Array<Pair_uint_vec3> positions;
    T_Append_Array<Pair_uint_vec3> rotations; // euler angles
    T_Append_Array<Pair_uint_vec3> scalings;
};

struct Node_Markers {
    ARRAY_HANDLE(Pair_uint_uint) anim;
    ARRAY_HANDLE(Pair_uint_uint) bone;
};

void AddAnimationKeys(Bone_Anim_Keys* outAnimKeys,
                      T_Append_Array<Pair_uint_vec3> animComponent,
                      float keyFrameTime) {
    Bone_Anim_Keys anim_keys = {
        GET_ARRAY_HANDLE(glm::vec3, animComponent.counter),
        GET_ARRAY_HANDLE(float, animComponent.counter),
    };

    for (uint32_t i = 0; i < animComponent.counter; i++) {
        (*anim_keys.key_frames)[i] = (*animComponent.handle)[i].key * keyFrameTime;
        (*anim_keys.values)[i] = (*animComponent.handle)[i].value;
    }

    *outAnimKeys = anim_keys;
}

// Helper functions internal to the file
void RemoveNodeAnimData(ARRAY_HANDLE(Node_Anim_Data) nodes) {
    size_t array_size = GET_ARRAY_SIZE(nodes);
    for(size_t i = 0; i < array_size; i++) {
        Node_Anim_Data node = (*nodes)[i];
        RETURN_ARRAY(node.positions.handle);
        RETURN_ARRAY(node.rotations.handle);
        RETURN_ARRAY(node.scalings.handle);
    }
    RETURN_ARRAY(nodes);
}

void GetBoneAnimation(Node_Anim_Data node, glm::mat4* bone, Transform* transform, Bone_Animation* boneAnimation, float keyFrameTime);

/// Traverses skeleton and calculates the global positions of the bones per key frame.
///
/// @param boneNodes     Handle to array of bones and values identified by key-frames.
/// @param numKeyFrames  Amount of key-frames.
/// @param meshRoot      Mesh root node.
/// @param boneRes       Mesh and animation resources for the skeleton.
///
/// @return the node key frame values sorted in the correct order with regards to the aiMesh bone array.
ARRAY_HANDLE(Node_Anim_Data) ImportGlobalBonesThroughTime(ARRAY_HANDLE(Node_Anim_Data) boneNodes,
                                                          uint32_t numKeyFrames,
                                                          aiNode* meshRoot,
                                                          Ai_Bone_Res boneRes);

void AddGlobalBonesForKeyFrame(aiNode* root,
                               aiMatrix4x4 parent,
                               Ai_Bone_Res boneRes,
                               ARRAY_HANDLE(Node_Anim_Data) nodes,
                               Node_Markers nodeMarkers,
                               uint32_t keyFrame,
                               uint32_t* animNodeCounter,
                               uint32_t* boneNodeCounter,
                               uint32_t* nodeCounter);

void AddGlobalBonesForFirstKeyFrame(aiNode* root,
                                    aiMatrix4x4 parent,
                                    Ai_Bone_Res boneRes,
                                    ARRAY_HANDLE(Node_Anim_Data) nodes,
                                    T_Append_Array<Pair_uint_uint>* animNodeMarkers,
                                    T_Append_Array<Pair_uint_uint>* boneNodeMarkers,
                                    uint32_t numKeyFrames,
                                    uint32_t* nodeCounter);

void UpdateParentMatrix(aiMatrix4x4* parent, const aiNodeAnim* nodeAnim, uint32_t keyFrame);
Transform GetTransform(const aiMatrix4x4& matrix);

const aiNodeAnim* FindNodeAnim(aiAnimation* animation,
                               const char* nodeName,
                               T_Append_Array<Pair_uint_uint>* animNodeMarkers,
                               uint32_t nodeCounter);

bool IsBone(aiMesh* mesh,
            const char* nodeName,
            T_Append_Array<Pair_uint_uint>* boneNodeMarkers,
            uint32_t nodeCounter);

const aiScene* ImportScene(Assimp::Importer* importer, std::string path)
{
    // Read file via ASSIMP
    const aiScene *scene = importer->ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    // Check for errors
    assert(scene && !scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE && scene->mRootNode);

    return scene;
}

std::vector<Default_Vertex> LoadVertices(aiMesh* mesh)
{
    std::vector<Default_Vertex> vertices;
    vertices.resize(mesh->mNumVertices);

    // Walk through each of the mesh's vertices
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Default_Vertex vertex;
        glm::vec3 vector;

        // Positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        // Normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        // Texture Coordinates
        if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coords = vec;
        }
        else
        {
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        }

        vertices[i] = vertex;
    }
    return vertices;
}

std::vector<uint32_t> LoadIndices(aiMesh* mesh)
{
    std::vector<uint32_t> indices;

    // Now walk	through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // Retrieve all indices of the face and store them in the indices std::vector
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    return indices;
}

// This function has to be adapted to the new structure of arrays - Textures
Textures LoadTextures(aiMesh* mesh, const aiScene* scene, std::string path, Textures texturesLoaded)
{
    Textures textures;

    std::string directory = path.substr(0, path.find_last_of('/'));

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        Textures diffuse_maps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuseSamp", directory, texturesLoaded);
        textures.id.insert(textures.id.end(), diffuse_maps.id.begin(), diffuse_maps.id.end());
        textures.path.insert(textures.path.end(), diffuse_maps.path.begin(), diffuse_maps.path.end());
        textures.types.insert(textures.types.end(), diffuse_maps.types.begin(), diffuse_maps.types.end());

        Textures spec_maps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "specularSamp", directory, texturesLoaded);
        textures.id.insert(textures.id.end(), spec_maps.id.begin(), spec_maps.id.end());
        textures.path.insert(textures.path.end(), spec_maps.path.begin(), spec_maps.path.end());
        textures.types.insert(textures.types.end(), spec_maps.types.begin(), spec_maps.types.end());

        Textures normal_maps = LoadMaterialTextures(material, aiTextureType_NORMALS, "normalSamp", directory, texturesLoaded);
        textures.id.insert(textures.id.end(), normal_maps.id.begin(), normal_maps.id.end());
        textures.path.insert(normal_maps.path.end(), normal_maps.path.begin(), normal_maps.path.end());
        textures.types.insert(normal_maps.types.end(), normal_maps.types.begin(), normal_maps.types.end());
    }

    return textures;
}

Textures LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory, Textures texturesLoaded)
{
    Textures textures;

    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;

        for (uint32_t i = 0; i < texturesLoaded.id.size(); i++)
        {
            if (texturesLoaded.path[i] == str)
            {
                textures.id.push_back(texturesLoaded.id[i]);
                textures.path.push_back(texturesLoaded.path[i]);
                textures.types.push_back(texturesLoaded.types[i]);

                // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                skip = true;

                break;
            }
        }

        if (!skip)
        {
            // If texture hasn't been loaded already, load it
            textures.id.push_back(TextureFromFile(str.C_Str() + '/' + directory, directory));
            textures.types.push_back(typeName);
            textures.path.push_back(str);
            // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            texturesLoaded.id.push_back(TextureFromFile(str.C_Str() + '/' + directory, directory));
            texturesLoaded.types.push_back(typeName);
            texturesLoaded.path.push_back(str);
        }
    }
    return textures;
}

uint32_t TextureFromFile(std::string fileName, std::string directory)
{
    int width, height, num_components;

    unsigned char* imageData = stbi_load(fileName.c_str(), &width, &height, &num_components, 4);

    uint32_t textureID;
    glGenTextures(1, &textureID);

    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(imageData);

    return textureID;
}

void DestroyTextures(Textures textures)
{
    for (uint32_t i = 0; i < textures.id.size(); i++)
    {
        glDeleteTextures(1, &textures.id[i]);
    }
}

// TODO: Add documentation for clarifying that this function depends on scene to mesh correspondence.
Skeleton LoadSkeleton(const aiScene* scene, ARRAY_HANDLE(Skeletal_Vertex)* outVertices) {
    aiMesh* mesh = scene->mMeshes[0];
    ARRAY_HANDLE(Skeletal_Vertex) vertices_handle = GET_ARRAY_HANDLE(Skeletal_Vertex, mesh->mNumVertices);
    
    assert(mesh->mNumVertices == GET_ARRAY_SIZE(vertices_handle));

    Skeletal_Vertex* vertices = *vertices_handle;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        // Positions
        vertices[i].position.x = mesh->mVertices[i].x;
        vertices[i].position.y = mesh->mVertices[i].y;
        vertices[i].position.z = mesh->mVertices[i].z;

        // Normals
        vertices[i].normal.x = mesh->mNormals[i].x;
        vertices[i].normal.y = mesh->mNormals[i].y;
        vertices[i].normal.z = mesh->mNormals[i].z;

        // Texture Coordinates
        if (mesh->mTextureCoords[0]) { // Does the mesh contain texture coordinates?
            // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vertices[i].tex_coords.x = mesh->mTextureCoords[0][i].x;
            vertices[i].tex_coords.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertices[i].tex_coords = glm::vec2(0.0f, 0.0f);
        }
        // The undefined values in the arrays in the skeletal vertex need to be zero to signify their non-assignment.
        memset(vertices[i].IDs, 0, sizeof(vertices[i].IDs));
        memset(vertices[i].weights, 0, sizeof(vertices[i].weights));
    }

    // The largest amount of a transform component's keys set the roof for the total amount of key-frames for the animation
    uint32_t num_pos_keys = scene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
    uint32_t num_rot_keys = scene->mAnimations[0]->mChannels[0]->mNumRotationKeys;
    uint32_t num_scale_keys = scene->mAnimations[0]->mChannels[0]->mNumScalingKeys;
    bool mostPositionKeys = num_pos_keys > num_rot_keys && num_pos_keys > num_scale_keys;
    bool most_rot_keys = num_rot_keys > num_pos_keys && num_rot_keys > num_scale_keys;
    bool most_scale_keys = num_scale_keys > num_pos_keys && num_scale_keys > num_rot_keys;
    uint32_t num_key_frames = (num_pos_keys * mostPositionKeys +
                             num_rot_keys * most_rot_keys +
                             most_scale_keys * num_scale_keys) +
        num_pos_keys * (!most_scale_keys && !most_rot_keys && !mostPositionKeys);

    // TODO: Support multiple animations per mesh. We should wait with this until we know what kinds of animations we have per type of mesh.
    // Doing one animation per mesh to begin with.
    Node_Anim_Data** nodes = GET_ARRAY_HANDLE(Node_Anim_Data, mesh->mNumBones);
    float anim_sec = (float) (scene->mAnimations[0]->mDuration / scene->mAnimations[0]->mTicksPerSecond);
    float sec_per_frame = anim_sec / (num_key_frames - 1);

    Print("Number of key frames %u\n", num_key_frames);
    Print("Seconds per key frame %.6f\n", sec_per_frame);
    Print("Animation second duration %.6f\n", anim_sec);

    nodes = ImportGlobalBonesThroughTime(nodes, num_key_frames, scene->mRootNode, { mesh, scene->mAnimations[0] });

    ARRAY_HANDLE(Bone_Animation) bone_anims = GET_ARRAY_HANDLE(Bone_Animation, mesh->mNumBones);
    ARRAY_HANDLE(Transform) bone_transforms = GET_ARRAY_HANDLE(Transform, mesh->mNumBones);
    ARRAY_HANDLE(glm::mat4) bones = GET_ARRAY_HANDLE(glm::mat4, mesh->mNumBones);

    for (uint32_t i = 0; i < mesh->mNumBones; i++) {
        GetBoneAnimation((*nodes)[i], (*bones) + i, (*bone_transforms) + i, (*bone_anims) + i, sec_per_frame);
        for (uint32_t j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
            uint32_t vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;

            // This loops looks for a free slot to fill in the weight array of the concerned vertex
            for (uint32_t k = 0; k < sizeof(vertices[vertex_id].IDs) / sizeof(vertices[vertex_id].IDs[0]); k++) {
                // 0.0 implies a free slot
                if (vertices[vertex_id].weights[k] == 0.0)
                {
                    vertices[vertex_id].IDs[k] = i;
                    vertices[vertex_id].weights[k] = weight;
                    break;
                }
            }
        }
    }
    // Remove what will not be needed outside of the function scope.
    RemoveNodeAnimData(nodes);

    *outVertices = vertices_handle;
    Skeleton skeleton = { bone_anims,
                          bone_transforms,
                          bones,
                          anim_sec };

    return skeleton;
}

void GetBoneAnimation(Node_Anim_Data node, glm::mat4* bone, Transform* transform, Bone_Animation* boneAnimation, float keyFrameTime) {
    bool animate_pos = node.positions.counter > 1;
    bool animate_rot = node.rotations.counter > 1;
    bool animate_scale = node.scalings.counter > 1;

    boneAnimation->animation_flags = GetAnimationFlags(animate_pos, animate_rot, animate_scale);

    if (animate_pos) {
        AddAnimationKeys(&boneAnimation->bone_position_keys, node.positions, keyFrameTime);
    }
    if (animate_rot) {
        AddAnimationKeys(&boneAnimation->bone_rotation_keys, node.rotations, keyFrameTime);
    }
    if (animate_scale) {
        AddAnimationKeys(&boneAnimation->bone_scale_keys, node.scalings, keyFrameTime);
    }

    *transform = { (*node.positions.handle)[0].value,
                   (*node.rotations.handle)[0].value,
                   (*node.scalings.handle)[0].value };

    // Add the starting bone to the bone array that will be used as the output destination.
    *bone = TransformMatrix(*transform);
}

ARRAY_HANDLE(Node_Anim_Data) ImportGlobalBonesThroughTime(ARRAY_HANDLE(Node_Anim_Data) boneNodes,
                                                          uint32_t numKeyFrames,
                                                          aiNode* meshRoot,
                                                          Ai_Bone_Res boneRes) {
    // Keep track of the order of animation and skeletal nodes to avoid searching for them more than one time.
    Node_Markers node_markers;
    node_markers.anim = GET_ARRAY_HANDLE(Pair_uint_uint, boneRes.animation->mNumChannels);
    node_markers.bone = GET_ARRAY_HANDLE(Pair_uint_uint, boneRes.mesh->mNumBones);

    aiMatrix4x4 identity_mat = aiMatrix4x4();
    uint32_t node_counter = 0;
    T_Append_Array<Pair_uint_uint> app_anim_markers  = { node_markers.anim, 0 };
    T_Append_Array<Pair_uint_uint> app_bone_markers = { node_markers.bone, 0 };
    // Record values for succeeding traversals of the node tree.
    AddGlobalBonesForFirstKeyFrame(meshRoot, identity_mat, boneRes, boneNodes,
                                   &app_anim_markers, &app_bone_markers, numKeyFrames, &node_counter);

    uint32_t anim_node_counter;
    uint32_t bone_node_counter;
    for(uint32_t i = FIRST_KEY_FRAME + 1; i < numKeyFrames; i++) {
        node_counter = 0;
        anim_node_counter = 0;
        bone_node_counter = 0;
        AddGlobalBonesForKeyFrame(meshRoot, identity_mat, boneRes, boneNodes, node_markers,
                                  i, &anim_node_counter, &bone_node_counter, &node_counter);
    }

    RETURN_ARRAY(node_markers.anim);
    RETURN_ARRAY(node_markers.bone);

    return boneNodes;
}

/// Sets the elements identified by keyFrame if it is not the first one, i.e. 0.
void AddGlobalBonesForKeyFrame(aiNode* root,
                               aiMatrix4x4 parent,
                               Ai_Bone_Res boneRes,
                               ARRAY_HANDLE(Node_Anim_Data) nodes,
                               Node_Markers nodeMarkers,
                               uint32_t keyFrame,
                               uint32_t* animNodeCounter,
                               uint32_t* boneNodeCounter,
                               uint32_t* nodeCounter) {
    // If all bones have been retrieved, return.
    size_t num_bones = GET_ARRAY_SIZE(nodeMarkers.bone);
    if (*boneNodeCounter >= num_bones) {
        return;
    }

    // If the animation node counter hasn't been surpassed by
    size_t num_anim_nodes = GET_ARRAY_SIZE(nodeMarkers.anim);
    if (num_anim_nodes > *animNodeCounter && *nodeCounter == (*nodeMarkers.anim)[*animNodeCounter].key) {
        uint32_t anim_node_id = (*nodeMarkers.anim)[*animNodeCounter].value;
        aiNodeAnim* node_anim = boneRes.animation->mChannels[anim_node_id];
        UpdateParentMatrix(&parent, node_anim, keyFrame);

        // Increment counter to compare with the next entry in the array for the next node.
        (*animNodeCounter)++;
    } else {
        parent *= root->mTransformation;
    }

    Pair_uint_uint bone_node_mark = (*nodeMarkers.bone)[*boneNodeCounter];
    if (*nodeCounter == bone_node_mark.key) {
        Node_Anim_Data node = (*nodes)[bone_node_mark.value];

        size_t prev_pos_id = TGetBack<>(node.positions);
        size_t prev_rot_id = TGetBack<>(node.rotations);
        size_t prev_scale_id = TGetBack<>(node.scalings);

        Pair_uint_vec3 prev_pos = (*node.positions.handle)[prev_pos_id];
        Pair_uint_vec3 prev_rot = (*node.rotations.handle)[prev_rot_id];
        Pair_uint_vec3 prev_scale = (*node.scalings.handle)[prev_scale_id];

        // Multiply by the offset of the bone to get global space values.
        aiMatrix4x4 globalParent = parent * boneRes.mesh->mBones[bone_node_mark.value]->mOffsetMatrix;
        Transform parentTransform = GetTransform(globalParent);

        // Only add the values if they have changed since the last key-frame.
        if (prev_pos.value != parentTransform.pos) {
            // If there has been more than one key-frame in between the differing values, an extra key-frame
            // has to be added such that the interpolation occurs between the last key-frame differing from
            // the new one being added.
            if (keyFrame - prev_pos.key > 1) {
                TAppendElement<>(&node.positions, { keyFrame - 1, prev_pos.value });
            }
            TAppendElement<>(&node.positions, { keyFrame, parentTransform.pos });
        }
        if (prev_rot.value != parentTransform.rot) {
            if (keyFrame - prev_rot.key > 1) {
                TAppendElement<>(&node.rotations, { keyFrame - 1, prev_rot.value });
            }
            TAppendElement<>(&node.rotations, { keyFrame, parentTransform.rot });
        }
        if (prev_scale.value != parentTransform.scale) {
            if (keyFrame - prev_scale.key > 1) {
                TAppendElement<>(&node.scalings, { keyFrame - 1, prev_scale.value });
            }
            TAppendElement<>(&node.scalings, { keyFrame, parentTransform.scale });
        }

        // We mustn't forget to write back to the real node instance, since we're dealing with copies.
        (*nodes)[bone_node_mark.value] = node;

        // Increment counter to compare with the next entry in the array for the next node.
        (*boneNodeCounter)++;
    }

    for (uint32_t i = 0; i < root->mNumChildren; i++)
    {
        (*nodeCounter)++;
        AddGlobalBonesForKeyFrame(root->mChildren[i],
                                  parent,
                                  boneRes,
                                  nodes,
                                  nodeMarkers,
                                  keyFrame,
                                  animNodeCounter,
                                  boneNodeCounter,
                                  nodeCounter);
    }
}

/// Sets the elements identified by the first keyFrame.
void AddGlobalBonesForFirstKeyFrame(aiNode* root,
                                    aiMatrix4x4 parent,
                                    Ai_Bone_Res boneRes,
                                    ARRAY_HANDLE(Node_Anim_Data) nodes,
                                    T_Append_Array<Pair_uint_uint>* animNodeMarkers,
                                    T_Append_Array<Pair_uint_uint>* boneNodeMarkers,
                                    uint32_t numKeyFrames,
                                    uint32_t* nodeCounter) {
    const char* node_name = root->mName.data;
    const aiNodeAnim* node_anim = FindNodeAnim(boneRes.animation, node_name, animNodeMarkers, *nodeCounter);

    // A future error may reside in that if the keys for each aspect of the transform are multiple and differ
    if (node_anim) {
        UpdateParentMatrix(&parent, node_anim, FIRST_KEY_FRAME);
    } else {
        parent *= root->mTransformation;
    }

    if (IsBone(boneRes.mesh, node_name, boneNodeMarkers, *nodeCounter)) {
        Node_Anim_Data node = {
            node_name,
            TGetAppendArray<Pair_uint_vec3>(numKeyFrames),
            TGetAppendArray<Pair_uint_vec3>(numKeyFrames),
            TGetAppendArray<Pair_uint_vec3>(numKeyFrames),
        };

        // The boneId is used to arrange the Node_Anim_Data in the same order as the assimp bone array, inherent to the aiMesh
        uint32_t bone_id = (*boneNodeMarkers->handle)[TGetBack<>(*boneNodeMarkers)].value;

        // Multiply by the offset of the bone to get global space values.
        aiMatrix4x4 global_parent = parent * boneRes.mesh->mBones[bone_id]->mOffsetMatrix;
        Transform parent_transform = GetTransform(global_parent);

        TAppendElement<>(&node.positions, { FIRST_KEY_FRAME, parent_transform.pos });
        TAppendElement<>(&node.rotations, { FIRST_KEY_FRAME, parent_transform.rot });
        TAppendElement<>(&node.scalings, { FIRST_KEY_FRAME, parent_transform.scale });

        (*nodes)[bone_id] = node;
    }

    for (uint32_t i = 0; i < root->mNumChildren; i++) {
        (*nodeCounter)++;
        AddGlobalBonesForFirstKeyFrame(root->mChildren[i],
                                       parent,
                                       boneRes,
                                       nodes,
                                       animNodeMarkers,
                                       boneNodeMarkers,
                                       numKeyFrames,
                                       nodeCounter);
    }
}

void UpdateParentMatrix(aiMatrix4x4* parent, const aiNodeAnim* nodeAnim, uint32_t keyFrame) {
    bool use_key_frame = nodeAnim->mNumScalingKeys != 1 && keyFrame < nodeAnim->mNumScalingKeys;
    aiVector3D scaling = nodeAnim->mScalingKeys[use_key_frame ? keyFrame : 0].mValue;

    use_key_frame = nodeAnim->mNumRotationKeys != 1 && keyFrame < nodeAnim->mNumRotationKeys;
    aiQuaternion rotation_q = nodeAnim->mRotationKeys[use_key_frame ? keyFrame : 0].mValue;

    use_key_frame = nodeAnim->mNumPositionKeys != 1 && keyFrame < nodeAnim->mNumPositionKeys;
    aiVector3D translation = nodeAnim->mPositionKeys[use_key_frame ? keyFrame : 0].mValue;

    // Refresh parent for next node
    *parent *= aiMatrix4x4(scaling, rotation_q, translation);
}

Transform GetTransform(const aiMatrix4x4& matrix) {
    // Decompose to store the individual vectors since we don't want to store the global bones as matrices, since
    // that would take up unnecessary space in memory.
    aiVector3D scaling;
    aiQuaternion rotation_q;
    aiVector3D translation;
    matrix.Decompose(scaling, rotation_q, translation);

    Transform transform;
    transform.scale.x = scaling.x;
    transform.scale.y = scaling.y;
    transform.scale.z = scaling.z;

    glm::quat quat = glm::quat(rotation_q.w, rotation_q.x, rotation_q.y, rotation_q.z);
    glm::vec3 euler_angles = glm::eulerAngles(quat);
    transform.rot.x = euler_angles.x;
    transform.rot.y = euler_angles.y;
    transform.rot.z = euler_angles.z;

    transform.pos.x = translation.x;
    transform.pos.y = translation.y;
    transform.pos.z = translation.z;

    return transform;
}

const aiNodeAnim* FindNodeAnim(aiAnimation* animation,
                               const char* nodeName,
                               T_Append_Array<Pair_uint_uint>* animNodeMarkers,
                               uint32_t nodeCounter) {
    for (uint32_t i = 0; i < animation->mNumChannels; i++)
    {
        aiNodeAnim* node_anim = animation->mChannels[i];

        if (strcmp(node_anim->mNodeName.data, nodeName) == 0)
        {
            // The order in which the nodes are traversed is stored in this array.
            // Next time the nodes are traversed, this functions does not need to
            // be called again, but instead a counter which is incremented for
            // every traversed node can be used for comparison against the next
            // value of the animNodeMarkers array.
            TAppendElement<>(animNodeMarkers, { nodeCounter, i });
            return node_anim;
        }
    }

    return NULL;
}

bool IsBone(aiMesh* mesh, const char* nodeName, T_Append_Array<Pair_uint_uint>* boneNodeMarkers, uint32_t nodeCounter) {
    for (uint32_t i = 0; i < mesh->mNumBones; i++) {
        if (strcmp(nodeName, mesh->mBones[i]->mName.data) == 0) {
            // The order in which the nodes are traversed is stored in this array.
            // Next time the nodes are traversed, this functions does not need to
            // be called again.
            TAppendElement<>(boneNodeMarkers, { nodeCounter, i });
            return true;
        }
    }
    return false;
}

Textures LoadMeshTextures(aiMesh* mesh, const aiScene* scene, std::string path)
{
    Textures textures;

    std::string directory = path.substr(0, path.find_last_of('/'));

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        Textures diffuse_maps = LoadMeshMaterialTextures(material, aiTextureType_DIFFUSE, "diffuseSamp", directory);
        textures.id.insert(textures.id.end(), diffuse_maps.id.begin(), diffuse_maps.id.end());
        textures.path.insert(textures.path.end(), diffuse_maps.path.begin(), diffuse_maps.path.end());
        textures.types.insert(textures.types.end(), diffuse_maps.types.begin(), diffuse_maps.types.end());

        Textures spec_maps = LoadMeshMaterialTextures(material, aiTextureType_SPECULAR, "specularSamp", directory);
        textures.id.insert(textures.id.end(), spec_maps.id.begin(), spec_maps.id.end());
        textures.path.insert(textures.path.end(), spec_maps.path.begin(), spec_maps.path.end());
        textures.types.insert(textures.types.end(), spec_maps.types.begin(), spec_maps.types.end());

        Textures normal_maps = LoadMeshMaterialTextures(material, aiTextureType_NORMALS, "normalSamp", directory);
        textures.id.insert(textures.id.end(), normal_maps.id.begin(), normal_maps.id.end());
        textures.path.insert(textures.path.end(), normal_maps.path.begin(), normal_maps.path.end());
        textures.types.insert(textures.types.end(), normal_maps.types.begin(), normal_maps.types.end());
    }

    return textures;
}

Textures LoadMeshMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory)
{
    Textures textures;

    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        textures.id.push_back(TextureFromFile(str.C_Str() + '/' + directory, directory));
        textures.types.push_back(typeName);
        textures.path.push_back(str);
    }
    return textures;
}
