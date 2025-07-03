#include <cstdint>
#include <iostream>
#include <climits>
#include "PrioritySet.h"

PrioritySet::PrioritySet(int k_value) : k(k_value) {
    size_t size_l3 = ((1ULL << (k * 3)) + 63) / 64;
    size_t size_l2 = ((1ULL << (k * 2)) + 63) / 64;
    size_t size_l1 = ((1ULL << k) + 63) / 64;

    level3.resize(size_l3);
    level2.resize(size_l2);
    level1.resize(size_l1);

    for (auto& l : level3) l = 0;
    for (auto& l : level2) l = 0;
    for (auto& l : level1) l = 0;
}

bool PrioritySet::insert(int value) {
    if (value < 0 || value >= (1 << (k * 3))) return false;

    int i3 = value / 64;
    int bit3 = value % 64;
    int i2 = value >> k;
    int i1 = value >> (k * 2);

    uint64_t mask3 = 1ULL << bit3;
    if (level3[i3] & mask3) {
        return false;
    }

    level3[i3] |= mask3;
    level2[i2 / 64] |= 1ULL << (i2 % 64);
    level1[i1 / 64] |= 1ULL << (i1 % 64);

    return true;
}

bool PrioritySet::remove(int value) {
    if (value < 0 || value >= (1 << (k * 3))) return false;

    int i3 = value / 64;
    int bit3 = value % 64;
    int i2 = value >> k;
    int i1 = value >> (k * 2);

    uint64_t mask3 = 1ULL << bit3;
    if (!(level3[i3] & mask3)) {
        return false;
    }

    locker.lock();

    level3[i3] &= ~mask3;

    bool block3_empty = true;
    for (int i = 0; i < 64; ++i) {
        if (level3[(i2 << k) + i]) {
            block3_empty = false;
            break;
        }
    }
    if (block3_empty)
        level2[i2 / 64] &= ~(1ULL << (i2 % 64));

    bool block2_empty = true;
    for (int i = 0; i < 64; ++i) {
        if (level2[(i1 << k) + i]) {
            block2_empty = false;
            break;
        }
    }
    if (block2_empty)
        level1[i1 / 64] &= ~(1ULL << (i1 % 64));

    locker.unlock();
    return true;
}

bool PrioritySet::has(int value) {
    if (value < 0 || value >= (1 << (k * 3))) return false;

    int i3 = value / 64;
    int bit3 = value % 64;
    return (level3[i3] >> bit3) & 1;
}

int PrioritySet::get_min() {
    locker.lock();

    for (size_t i1 = 0; i1 < level1.size(); ++i1) {
        uint64_t l1 = level1[i1];
        if (l1 == 0) continue;

        int b1 = __builtin_ctzll(l1);
        int group1 = i1 * 64 + b1;

        for (size_t i2 = group1 << k; i2 < ((group1 + 1) << k); ++i2) {
            if (i2 >= level2.size()) break;
            uint64_t l2 = level2[i2];
            if (l2 == 0) continue;

            int b2 = __builtin_ctzll(l2);
            int group2 = i2 * 64 + b2;

            for (size_t i3 = group2 << k; i3 < ((group2 + 1) << k); ++i3) {
                if (i3 >= level3.size()) break;
                uint64_t l3 = level3[i3];
                if (l3 == 0) continue;

                int b3 = __builtin_ctzll(l3);
                locker.unlock();
                return static_cast<int>(i3 * 64 + b3);
            }
        }
    }

    locker.unlock();
    return -1;
}

int PrioritySet::pop_min() {
    int val = get_min();
    if (val != -1)
        remove(val);
    return val;
}

void PrioritySet::print() {
    std::cout << "PrioritySet contents:" << std::endl;
    for (size_t i = 0; i < level3.size(); ++i) {
        uint64_t l3 = level3[i];
        if (l3 != 0) {
            std::cout << "Level 3 block " << i << ": ";
            for (int j = 0; j < 64; ++j) {
                if (l3 & (1ULL << j)) {
                    std::cout << (i * 64 + j) << " ";
                }
            }
            std::cout << std::endl;
        }
    }
}