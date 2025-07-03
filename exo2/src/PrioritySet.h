#pragma once
#include <atomic>
#include <mutex>

class PrioritySet
{
    public:
    int k = 2;
    std::atomic<uint64_t> level3{0};
    std::atomic<uint16_t> level2{0};
    std::atomic<uint8_t>  level1{0};

    std::mutex locker;

    // Insère 'value' s'il n'est pas déjà présent
    // Retourne true si l'insertion a été faite, false sinon
    bool insert(int value);

    // Supprime 'value' s'il est présent
    // Retourne true si l'élément a été supprimé, false sinon
    bool remove(int value);

    // Retourne true si l'élément est présent
    bool has(int value);

    // Retourne l'élément avec la plus petite valeur (sans le retirer)
    int get_min();

    // Retourne et supprime l'élément avec la plus petite valeur
    int pop_min();

    void print();
};