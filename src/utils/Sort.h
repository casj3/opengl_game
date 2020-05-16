#pragma once

#include <LoadVAO.h>
#include "Allocator.h"

namespace Sort {

uint32_t GetMaxPairValue(ArrayHandle<Pair<uint32_t, uint32_t>> pairs, uint32_t size);

template <typename T>
void CountSortArrayLikePairs(ArrayHandle<Pair<uint32_t, uint32_t>> pairs,
                             ArrayHandle<T> array,
                             uint32_t size,
                             uint32_t step) {
    ArrayHandle<Pair<uint32_t, uint32_t>> pairOutput = Allocator::AddArray<Pair<uint32_t, uint32_t>>(size);
    ArrayHandle<T> arrayOutput = Allocator::AddArray<T>(size);

    constexpr uint32_t kRadixCountMax = 10;
    uint32_t counts[kRadixCountMax] = {0};

    // Calculate amount of numbers in range 0-9 for the given step. The total amount
    // can not be more than the size of the array.
    for (uint32_t i = 0; i < size; i++) {
        Pair<uint32_t, uint32_t> pair = Allocator::GetElement<>(pairs, i);
        counts[(pair.value / step) % kRadixCountMax]++;
    }

    // Calculate cummulative amount to let the elements reflect the order based
    // on the size they signify, for the given step.
    for (uint32_t i = 1; i < kRadixCountMax; i++) {
        counts[i] += counts[i - 1];
    }

    // Calculate amount of numbers in range 0-9 for the given step.
    for (uint32_t i = size - 1; i >= 0; i--) {
        Pair<uint32_t, uint32_t> pair = Allocator::GetElement<>(pairs, i);
        uint32_t countId = (pair.value / 10) % kRadixCountMax;
        Allocator::SetElement<>(pairOutput, counts[countId] - 1, pair);

        // Match the order of the generic elements with the sorted pairs.
        T element = Allocator::GetElement<>(array, i);
        Allocator::SetElement<>(arrayOutput, counts[countId] - 1, element);

        // Decrement the count to place the next corresponding element at a lower position in the array.
        counts[countId]--;
    }

    for (uint32_t i = 0; i < size; i++) {
        Pair<uint32_t, uint32_t> outputPair = Allocator::GetElement<>(pairOutput, i);
        T outputElement = Allocator::GetElement<>(arrayOutput, i);

        Allocator::SetElement<>(pairs, i, outputPair);
        Allocator::SetElement<>(array, i, outputElement);
    }

    Allocator::RemoveArray<>(pairOutput);
    Allocator::RemoveArray<>(arrayOutput);
}

/// Sorts the array with respect to value of the pair.
///
/// @param pairs The pair array to sort.
/// @param array A generic array sorted in the same way as the pairs.
///
/// @returns the order in which it was sorted.
template <typename T>
void RadixSortArrayLikePairs(ArrayHandle<Pair<uint32_t, uint32_t>> pairs, ArrayHandle<T> array, uint32_t size) {
    uint32_t maxValue = GetMaxPairValue(pairs, size);

    for (uint32_t step = 1; maxValue/step > 0; step *= 10) {
        CountSortArrayLikePairs(pairs, array, size, step);
    }
}

}
