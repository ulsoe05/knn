#pragma once
#include <cstdint>
#include <vector>

// Popular one
// https://gist.github.com/badboy/6267743
uint32_t wang_hash(uint32_t seed){
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

float wang_hash_float(uint32_t seed){
    return (float)wang_hash(seed) / (float)UINT32_MAX;
}


// https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
// This one should be faster and better in every way
uint32_t pcg_hash(uint32_t input){
    input = input * 747796405u + 2891336453u;
    uint32_t word = ((input >> ((input >> 28u) + 4u)) ^ input) * 277803737u;
    return (word >> 22u) ^ word;
}

float pcg_hash_float(uint32_t input){
    return (float)pcg_hash(input) / (float)UINT32_MAX;
}

template <typename T>
void shuffle_durstenfeld(std::vector<T>& a, uint32_t seed){
    for (size_t i = a.size() - 1; i > 0; i--){
        size_t j = pcg_hash((uint32_t)i + seed) % (i + 1);
        std::swap(a[i], a[j]);
    }
}