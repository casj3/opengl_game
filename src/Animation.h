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

/// Maps skeletons to vertex array objects.
struct Skeletons {
  uint32_t skeleton_array_id;
  uint32_t vao_array_id;
};

/// Maps bones to starting values for all bones.
struct Skeleton {
  uint32_t bone_array_id;

  uint32_t default_position_array_id;
  uint32_t default_rotation_array_id;
  uint32_t default_scale_array_id;
};

/// Maps skeletons to position arrays to see what
/// skeletons animate in position.
struct PositionSkeletons {
  uint32_t skeleton_array_id;
  uint32_t skeleton_position_array_id;
};

/// Maps skeletons to rotation arrays to see what
/// skeletons animate in rotation.
struct RotationSkeletons {
  uint32_t skeleton_array_id;
  uint32_t skeleton_rotation_array_id;
};

/// Maps skeletons to scale arrays to see what
/// skeletons animate in scale.
struct ScaleSkeletons {
  uint32_t skeleton_array_id;
  uint32_t skeleton_scale_array_id;
};

/// Maps bone array to bone position animations.
struct SkeletonPositions {
  /// ID of array to match a bone with a start-index in an array with all
  /// different key-frame corresponding positions for all bones.
  uint32_t bone_array_id;

  /// The starting index in the array per bone.
  /// The ending index is the next entry in the column.
  uint32_t bone_position_start_key_array_id;
};

/// Maps bone array to bone rotation animations.
struct SkeletonRotations {
  /// ID of array to match a bone with a start-index in an array with all
  /// different key-frame corresponding rotations for all bones.
  uint32_t bone_array_id;

  /// The starting index in the array per bone.
  /// The ending index is the next entry in the column/array.
  uint32_t bone_rotation_start_key_array_id;
};

/// Maps bone array to bone scale animations.
struct SkeletonScales {
  /// ID of array to match a bone with a start-index in an array with all
  /// different key-frame corresponding scale values for all bones.
  uint32_t bone_array_id;

  /// The starting index in the array per bone.
  /// The ending index is the next entry in the column/array.
  uint32_t bone_scale_start_key_array_id;
};

/// Maps position values to key frames.
struct BonePositionKeys {
  uint32_t position_array_id;
  uint32_t key_frame_array_id;
};

/// Maps rotation values to key frames.
struct BoneRotationKeys {
  uint32_t rotation_array_id;
  uint32_t key_frame_array_id;
};

/// Maps scale values to key frames.
struct BoneScaleKeys {
  uint32_t scale_array_id;
  uint32_t key_frame_array_id;
};

// This method presupposes an exact correlation between animation keys and
// the indicies of the matrix std::vector within the vector
void SetAnimationBones(float animationTime, SkeletonSuper* animSuper);

// The key_bone_frames mustn't be deallocated because it is a std::vector
void DestroySkeleton(glm::mat4* animaitonBoneTransforms);
