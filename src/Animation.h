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
    NONE = 0,
    POSITION = 1,
    ROTATION = 1 << 1,
    SCALE = 1 << 2,
};

/// Maps values of a bone animation component, e.g. scale, to key frames.
struct BoneAnimKeys {
    ArrayHandle<glm::vec3> values;
    ArrayHandle<float> key_frames;
};

struct BoneAnimation {
    BoneAnimKeys bone_position_keys;
    BoneAnimKeys bone_rotation_keys;
    BoneAnimKeys bone_scale_keys;
    /// Indicates what transform component keys the bone contains.
    /// The array handle is only valid if the corresponding flag
    /// is set.
    int32_t animation_flags;
};

struct Skeleton {
    /// Iput for writing to bones.
    ArrayHandle<BoneAnimation> bone_anims;
    ArrayHandle<glm::mat4> bones;
    /// Signifies the transform components animating the skeleton.
    int32_t animation_flags;
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

/// Generats an integer that can be compared against the AnimationFlags to
/// determine if position, rotation or scale has been set.
///
/// @param position Condition for the position flag.
/// @param position Condition for the rotation flag.
/// @param position Condition for the scale flag.
///
/// @returns An integer holding flags defined by the AnimationFlags enum.
int32_t GetAnimationFlags(bool position, bool rotation, bool scale);
