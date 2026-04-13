#pragma once
#include <random>
inline std::mt19937& get_rng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}