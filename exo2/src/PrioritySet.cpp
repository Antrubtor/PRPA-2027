#include "PrioritySet.h"

bool PrioritySet::insert(int value) 
{
    if (this == null)
    {
        this = new PrioritySet(value);
        return true;   
    }
    
}