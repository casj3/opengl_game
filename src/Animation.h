#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Transform.h"
#include <utils/ArrayManager.h>

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
    
    /// Start values of bone transform components, in case a
    /// component doesn't animate for a given bone.
    ArrayHandle<Transform> bone_transforms;

    /// Output.
    ArrayHandle<glm::mat4> bones;

    // Animation duration in seconds.
    float animation_duration;
};

/// Maps skeletons to vertex array objects. This structure should not be in an array.
/// There should only exist one instance of this structure in the application.
struct Skeletons {
    ArrayHandle<Skeleton> skeletons_array;
    ArrayHandle<uint32_t> vao_array;
    ArrayHandle<uint32_t> element_buffer_sizes;
};

void AnimateBones(float animationTime, Skeleton skeleton);

/// Generats an integer that can be compared against the AnimationFlags to
/// determine if position, rotation or scale has been set.
///
/// @param position Condition for the position flag.
/// @param position Condition for the rotation flag.
/// @param position Condition for the scale flag.
///
/// @returns An integer holding flags defined by the AnimationFlags enum.
int32_t GetAnimationFlags(bool position, bool rotation, bool scale);
