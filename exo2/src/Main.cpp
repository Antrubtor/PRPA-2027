#include "PrioritySet.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    PrioritySet ps(1);
    ps.insert(2);
    ps.insert(3);
    ps.insert(4);

    for (int i = 1; i <= 7; ++i)
    {
        if (ps.has(i))
        {
            std::cout << "PrioritySet contains: " << i << std::endl;
        }
        else
        {
            std::cout << "PrioritySet does not contain: " << i << std::endl;
        }
    }

    ps.remove(3);
    std::cout << ps.has(3) << std::endl;
    return 0;
}
