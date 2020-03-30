#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <utils/Allocator.h>

struct SkeletonSuper
{
    std::vector<std::vector<glm::mat4>> key_bone_frames;
    glm::mat4* animation_bone_transforms = nullptr;
    int bone_amount;
    // Not sure addition
    int entity_id;
};

enum AnimationFlags {
    POSITION = 0 << 1,
    ROTATION = 0 << 2,
    SCALE = 0 << 3,
};

/// Maps position values of bone to key frames.
struct BonePositionKeys {
    Allocator::ArrayHandle<glm::vec3> position_array_id;
    Allocator::ArrayHandle<float> key_frame_array_id;
};

/// Maps rotation values of a bone to key frames.
struct BoneRotationKeys {
    Allocator::ArrayHandle<glm::vec3> rotation_array_id;
    Allocator::ArrayHandle<float> key_frame_array_id;
};

/// Maps scale values of a bone to key frames.
struct BoneScaleKeys {
    Allocator::ArrayHandle<glm::vec3> scale_array_id;
    Allocator::ArrayHandle<float> key_frame_array_id;
};

/// Maps bone IDs (of type uint32_t) to bone BonePositionKeys.
struct SkeletonPositions {
    Allocator::ArrayHandle<uint32_t> position_bone_array_id;
    Allocator::ArrayHandle<BonePositionKeys> bone_position_keys_id;
};

/// Maps bone IDs (of type uint32_t) to BoneRotationKeys.
struct SkeletonRotations {
    Allocator::ArrayHandle<uint32_t> rotation_bone_array_id;
    Allocator::ArrayHandle<BoneRotationKeys> bone_rotation_keys_id;
};

/// Maps bone IDs (of type uint32_t) to BoneScaleKeys.
struct SkeletonScales {
    Allocator::ArrayHandle<uint32_t> scale_bone_array_id;
    Allocator::ArrayHandle<BoneScaleKeys> bone_scale_keys_id;
};

/// Maps bone ID arrays, i.e. skeletons, to position arrays to see what
/// skeletons animate in position.
/// This structure should not be in an array.
struct PositionSkeletons {
    Allocator::ArrayHandle<uint32_t> skeleton_array_id;
    Allocator::ArrayHandle<SkeletonRotations> skeleton_position_array_id;
};

/// Maps ID arrays, i.e. skeletons, to rotation arrays to see what
/// skeletons animate in rotation.
/// This structure should not be in an array.
struct RotationSkeletons {
    Allocator::ArrayHandle<uint32_t> skeleton_array_id;
    Allocator::ArrayHandle<SkeletonRotations> skeleton_rotation_array_id;
};

/// Maps ID arrays, i.e. skeletons, to scale arrays to see what
/// skeletons animate in scale.
/// This structure should not be in an array.
struct ScaleSkeletons {
    Allocator::ArrayHandle<uint32_t> skeleton_array_id;
    Allocator::ArrayHandle<SkeletonScales> skeleton_scale_array_id;
};

/// Maps skeletons (bone IDs (of type uint32_t)) to vertex array objects.
/// This structure should not be in an array. There should only exist one
/// instance of this structure in the program.
struct Skeletons {
    Allocator::ArrayHandle<uint32_t> skeleton_array_id;
    
    Allocator::ArrayHandle<glm::vec3> default_position_array_id;
    Allocator::ArrayHandle<glm::vec3> default_rotation_array_id;
    Allocator::ArrayHandle<glm::vec3> default_scale_array_id;

    Allocator::ArrayHandle<uint32_t> vao_array_id;

    /// Includes flags from enum SkeletonArrays or is 0.
    /// Each flag indicates which skeletons (position, rotation or scale)
    /// the skeleton with the corresponding bone_array_id belongs to, which
    /// could be all of them. In other words, its amount is always the same
    /// as the array references by the bone_array_id.
    Allocator::ArrayHandle<int32_t> array_types_flag;

    PositionSkeletons positionSkeletons;
    RotationSkeletons rotationSkeletons;
    ScaleSkeletons scaleSkeletons;
};

/* The structures below determines how a skeleton should be animated. */

// This method presupposes an exact correlation between animation keys and
// the indicies of the matrix std::vector within the vector
void SetAnimationBones(float animationTime, SkeletonSuper* animSuper);

// The key_bone_frames mustn't be deallocated because it is a std::vector
void DestroySkeleton(glm::mat4* animaitonBoneTransforms);
