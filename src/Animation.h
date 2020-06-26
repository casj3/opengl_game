#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <utils/Allocator.h>

// TODO: Remove once the other structures, below, are put to use.
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
    ArrayHandle<glm::vec3> positions;
    ArrayHandle<float> key_frames;
};

/// Maps rotation values of a bone to key frames.
struct BoneRotationKeys {
    ArrayHandle<glm::vec3> rotations;
    ArrayHandle<float> key_frames;
};

/// Maps scale values of a bone to key frames.
struct BoneScaleKeys {
    ArrayHandle<glm::vec3> scalings;
    ArrayHandle<float> key_frames;
};

struct BoneAnimation {
    ArrayHandle<BonePositionKeys> bone_position_keys;
    ArrayHandle<BoneRotationKeys> bone_rotation_keys;
    ArrayHandle<BoneScaleKeys> bone_scale_keys;
    /// Indicates what transform component keys the bone contains.
    /// The array handle is only valid if the corresponding flag
    /// is set.
    AnimationFlags animation_flags;
};

struct Skeleton {
    /// Iput for writing to bones.
    ArrayHandle<BoneAnimation> bone_anims;
    ArrayHandle<glm::mat4> bones;
    /// Signifies the transform components animating the skeleton.
    AnimationFlags animation_flags;
};

/// Maps skeletons to vertex array objects. This structure should not be in an array.
/// There should only exist one instance of this structure in the application.
struct Skeletons {
    ArrayHandle<Skeleton> skeletons;
    ArrayHandle<uint32_t> vao_array_id;
};

/* TODO: Rewrite the functions below to work with the new structures. */

// This method presupposes an exact correlation between animation keys and
// the indicies of the matrix std::vector within the vector
void SetAnimationBones(float animationTime, SkeletonSuper* animSuper);

// The key_bone_frames mustn't be deallocated because it is a std::vector
void DestroySkeleton(glm::mat4* animaitonBoneTransforms);
