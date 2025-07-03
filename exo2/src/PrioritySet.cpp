#include <cstdint>
#include <iostream>
#include <climits>
#include "PrioritySet.h"

bool PrioritySet::insert(int value) {
    if (value < 0 || value > 63) return false;
        uint64_t mask3 = 1ULL << value;
        int idx2 = value / 4;
        int idx1 = value / 16;
        // Vérifier présence
        if (level3.load() & mask3) return false;

        // Insertion atomique
        level3.fetch_or(mask3);
        level2.fetch_or(1U << idx2);
        level1.fetch_or(1U << idx1);

        return true;
}

bool PrioritySet::remove(int value) {
    if (value < 0 || value > 63) return false;

    uint64_t mask3 = 1ULL << value;
    int block2 = value / 4;
    int block1 = value / 16;

    // Vérifie si absent
    if (!(level3.load(std::memory_order_relaxed) & mask3))
        return false;

    locker.lock();
    // Éteint le bit
    level3.fetch_and(~mask3, std::memory_order_relaxed);

    // Nettoyage conditionnel niveau 2
    uint64_t block_mask3 = 0xFULL << (block2 * 4);
    if ((level3.load() & block_mask3) == 0)
        level2.fetch_and(~(1U << block2), std::memory_order_relaxed);

    // Nettoyage conditionnel niveau 1
    uint16_t block_mask2 = 0xFU << (block1 * 4);
    if ((level2.load() & block_mask2) == 0)
        level1.fetch_and(~(1U << block1), std::memory_order_relaxed);

    locker.unlock();
    return true;
}

bool PrioritySet::has(int value) {
    if (value < 0 || value > 63) return false;
    return (level3.load(std::memory_order_relaxed) >> value) & 1;
}

int PrioritySet::get_min() {
    uint8_t l1 = level1.load(std::memory_order_relaxed);
    if (l1 == 0) return -1;

    int i1 = __builtin_ctz(l1);
    uint16_t l2 = level2.load(std::memory_order_relaxed);
    int i2 = __builtin_ctz((l2 >> (i1 * 4)) & 0xF);
    uint64_t l3 = level3.load(std::memory_order_relaxed);
    int i3 = __builtin_ctz((l3 >> (i1 * 16 + i2 * 4)) & 0xF);

    return i1 * 16 + i2 * 4 + i3;
}

int PrioritySet::pop_min() {
    int val = get_min();
    if (val != -1)
        remove(val);
    return val;
}

void PrioritySet::print() {
    std::cout << "PrioritySet: ";
    for (int i = 0; i < 64; ++i) {
        if (has(i)) {
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
}