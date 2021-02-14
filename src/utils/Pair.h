#pragma once

#include <stdint.h>

#include <GL/glew.h>

#define DECL_PAIR_TYPE(key_t, value_t, suffix)        \
    struct Pair_##suffix {                            \
        key_t key;                                    \
        value_t value;                                \
    };

DECL_PAIR_TYPE(uint32_t, uint32_t, uint_uint)
DECL_PAIR_TYPE(uint32_t, glm::vec3, uint_vec3)
