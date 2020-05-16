
#include "Sort.h"

namespace Sort {

uint32_t GetMaxPairValue(ArrayHandle<Pair<uint32_t, uint32_t>> pairs, uint32_t size) {
    uint32_t max = Allocator::GetElement<Pair<uint32_t, uint32_t>>(pairs, 0).value;

    for (uint32_t i = 0; i < size; i++) {
        uint32_t element = Allocator::GetElement<Pair<uint32_t, uint32_t>>(pairs, i).value;
        if (element > max) {
            max = element;
        }
    }

    return max;
}

}
