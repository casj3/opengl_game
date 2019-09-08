#pragma once

#include <glm/glm.hpp>
#include <vector>

struct SkeletonSuper
{
	std::vector<std::vector<glm::mat4>> key_bone_frames;
	glm::mat4* animation_bone_transforms = nullptr;
	int bone_amount;
  // Not sure addition
  int entity_id;
};

struct Skeletons {
  uint32_t* skeleton_ID;
  uint32_t* vao_ID;

  uint32_t rows;
};

// Accessed with skeleton_ID from array
struct Skeleton {
  uint32_t* bone_ID;
  float* positions[3];
  float* rotations[3];
  float* scales[3];

  uint32_t rows;
};

struct SkeletonPositions {
  uint32_t* bone_ID;
  // The starting index in the entire table.
  // The ending index is the next entry in the column.
  uint32_t* bone_position_keys_start_index;

  uint32_t rows;
};

struct BonePositionKeys {
  float* positions[3];
  float* key_frames;

  uint32_t rows;
};

struct SkeletonRotations {
  uint32_t* bone_ID;
  // The starting index in the entire table.
  // The ending index is the next entry in the column.
  uint32_t* bone_rotation_keys_start_index;

  uint32_t rows;
};

struct BoneRotationKeys {
  float* rotations[3];
  float* key_frames;

  uint32_t rows;
};

struct SkeletonScales {
  uint32_t* bone_ID;
  // The starting index in the entire table.
  // The ending index is the next entry in the column.
  uint32_t* bone_scale_keys_start_index;

  uint32_t rows;
};

struct BoneScaleKeys {
  float* scales[3];
  float* key_frames;

  uint32_t rows;
};

// This method presupposes an exact correlation between animation keys and the indicies of the matrix std::vector within the vector
void SetAnimationBones(float animationTime, SkeletonSuper* animSuper);

// The key_bone_frames mustn't be deallocated because it is a std::vector
void DestroySkeleton(glm::mat4* animaitonBoneTransforms);
