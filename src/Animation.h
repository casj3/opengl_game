#pragma once

#include <glm/glm.hpp>
#include <vector>

enum SkeletonArrays {
    POSITION_ARRAY = 0 << 1,
    ROTATION_ARRAY = 0 << 2,
    SCALE_ARRAY = 0 << 3,
};

struct SkeletonSuper
{
    std::vector<std::vector<glm::mat4>> key_bone_frames;
    glm::mat4* animation_bone_transforms = nullptr;
    int bone_amount;
    // Not sure addition
    int entity_id;
};

/// Maps bone ID arrays, i.e. skeletons, to position arrays to see what
/// skeletons animate in position.
/// This structure should not be in an array.
struct PositionSkeletons {
    uint32_t bone_array_id;
    /// ID to array of type SkeletonPositions.
    uint32_t skeleton_position_array_id;
};

/// Maps ID arrays, i.e. skeletons, to rotation arrays to see what
/// skeletons animate in rotation.
/// This structure should not be in an array.
struct RotationSkeletons {
    uint32_t bone_array_id;
    /// ID to array of type SkeletonRotations.
    uint32_t skeleton_rotation_array_id;
};

/// Maps ID arrays, i.e. skeletons, to scale arrays to see what
/// skeletons animate in scale.
/// This structure should not be in an array.
struct ScaleSkeletons {
    uint32_t bone_array_id;
    /// ID to array of type SkeletonScales.
    uint32_t skeleton_scale_array_id;
};

/// Maps skeletons (bone IDs (of type uint32_t)) to vertex array objects.
/// This structure should not be in an array. There should only exist once
/// instance of this structure in the program.
struct Skeletons {
    uint32_t bone_array_id;
    uint32_t default_position_array_id;
    uint32_t default_rotation_array_id;
    uint32_t default_scale_array_id;

    uint32_t vao_array_id;
    /// Includes flags from enum SkeletonArrays or is 0.
    /// Each flag indicates which skeletons (position, rotation or scale)
    /// the skeleton with the corresponding bone_array_id belongs to, which
    /// could be all of them. In other words, its amount is always the same
    /// as the array references by the bone_array_id.
    int32_t array_types_flag;

    struct PositionSkeletons positionSkeletons;
    struct RotationSkeletons rotationSkeletons;
    struct ScaleSkeletons scaleSkeletons;
};

/* The structures below determines how a skeleton should be animated. */

/// Maps bone IDs (of type uint32_t) to bone position animations.
struct SkeletonPositions {
    /// ID of array to match a bone with a start-index in an array with all
    /// different key-frame corresponding positions for all bones.
    uint32_t position_bone_array_id;

    /// The starting index in the array per bone.
    /// The ending index is the next entry in the column.
    uint32_t bone_position_start_key_array_id;
};

/// Maps bone IDs (of type uint32_t) to bone rotation animations.
struct SkeletonRotations {
    /// ID of array to match a bone with a start-index in an array with all
    /// different key-frame corresponding rotations for all bones.
    uint32_t rotation_bone_array_id;

    /// The starting index in the array per bone.
    /// The ending index is the next entry in the column/array.
    uint32_t bone_rotation_start_key_array_id;
};

/// Maps bone IDs (of type uint32_t) to bone scale animations.
struct SkeletonScales {
    /// ID of array to match a bone with a start-index in an array with all
    /// different key-frame corresponding scale values for all bones.
    uint32_t scale_bone_array_id;

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
