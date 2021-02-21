#pragma once

#include <glm/glm.hpp>

#include "transform.h"
#include "utils/array_pool.h"

enum Anim_Flags {
    NONE = 0,
    POSITION = 1,
    ROTATION = 1 << 1,
    SCALE = 1 << 2,
};

/// Maps values of a bone animation component, e.g. scale, to key frames.
struct Bone_Anim_Keys {
    ARRAY_HANDLE(glm::vec3) values;
    ARRAY_HANDLE(float) key_frames;
};

struct Bone_Animation {
    Bone_Anim_Keys bone_position_keys;
    Bone_Anim_Keys bone_rotation_keys;
    Bone_Anim_Keys bone_scale_keys;
    /// Indicates what transform component keys the bone contains.
    /// The array handle is only valid if the corresponding flag
    /// is set.
    int32_t animation_flags;
};

struct Skeleton {
    /// Iput for writing to bones.
    ARRAY_HANDLE(Bone_Animation) bone_anims;
    
    /// Start values of bone transform components, in case a
    /// component doesn't animate for a given bone.
    ARRAY_HANDLE(Transform) bone_transforms;

    /// Output.
    ARRAY_HANDLE(glm::mat4) bones;

    // Animation duration in seconds.
    float animation_duration;
};

/// Maps skeletons to vertex array objects. This structure should not be in an array.
/// There should only exist one instance of this structure in the application.
struct Skeletons {
    ARRAY_HANDLE(Skeleton) skeletons_array;
    ARRAY_HANDLE(uint32_t) vao_array;
    ARRAY_HANDLE(uint32_t) element_buffer_sizes;
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
