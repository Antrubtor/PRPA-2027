#pragma once
#include <atomic>
#include <mutex>
#include <vector>

class PrioritySet
{
    public:
    int k;
    std::vector<uint64_t> level3;
    std::vector<uint64_t> level2;
    std::vector<uint64_t> level1;

    std::mutex locker;

    PrioritySet(int k_value = 6);

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