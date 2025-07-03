#include <iostream>
#include <thread>

#include "PrioritySet.h"

void inserter(PrioritySet& ps, int begin, int end) {
    for (int i = begin; i <= end; i++) {
        if (ps.insert(i)) {
            std::cout << "Inserted: " << i << std::endl;
        } else {
            std::cout << "Failed to insert: " << i << std::endl;
        }
    }
}

void remover(PrioritySet& ps, int begin, int end) {
    for (int i = begin; i <= end; i++) {
        if (ps.remove(i)) {
            std::cout << "Removed: " << i << std::endl;
        } else {
            std::cout << "Failed to remove: " << i << std::endl;
        }
    }
}

void min_popper(PrioritySet& ps, int id) {
    int min_val = ps.pop_min();
    if (min_val != -1) {
        std::cout << id << ": Popped min: " << min_val << std::endl;
    } else {
        std::cout << id << ": No elements to pop." << std::endl;
    }
}


int main() {
    PrioritySet ps;

    std::thread t1(inserter, std::ref(ps), 0, 10);
    t1.join(); 
    std::thread t2(min_popper, std::ref(ps), 1);
    std::thread t6(min_popper, std::ref(ps), 2);
    std::thread t7(min_popper, std::ref(ps), 3);
    std::thread t3(min_popper, std::ref(ps), 4);
    std::thread t8(min_popper, std::ref(ps), 5);
    std::thread t9(min_popper, std::ref(ps), 6);
    std::thread t4(min_popper, std::ref(ps), 7);
    std::thread t5(min_popper, std::ref(ps), 8);
    std::thread t10(min_popper, std::ref(ps), 9);
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join(); 
    t7.join();
    t8.join();
    t9.join();
    t10.join();
    
    ps.print();
}
