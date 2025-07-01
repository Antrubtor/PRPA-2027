#include "PrioritySet.h"

bool PrioritySet::insert(int value) 
{
    PrioritySet tmp = this;
    while (tmp.next != null)
    {
        if (tmp.val == value)
        {
            return false;
        }
        tmp = tmp.next;
    }
    if (tmp.val == value)
    {
        return false;
    }
    PrioritySet n = new PrioritySet(value);
    tmp.next = n;
    return true;
}

bool PrioritySet::remove(int value)
{
    PrioritySet tmp = this;
    while (tmp != null)
    {
        if (tmp.val == value)
        {
            tmp.next = tmp.next.next;
            return true;
        }
    }
    return false;
}

bool PrioritySet::has(int value)
{
    PrioritySet tmp = this;
    while (tmp != null)
    {
        if (tmp.val == value)
        {
            return true;
        }
    }
    return false;
}

