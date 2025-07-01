#include "PrioritySet.h"


PrioritySet::PrioritySet(int value):val(value), next(nullptr) {};

bool PrioritySet::insert(int value) 
{
    PrioritySet* tmp = this;
    while (tmp->next != nullptr)
    {
        if (tmp->val == value)
        {
            return false;
        }
        tmp = tmp->next;
    }
    if (tmp->val == value)
    {
        return false;
    }
    PrioritySet* n = new PrioritySet(value);
    tmp->next = n;
    return true;
}

bool PrioritySet::remove(int value)
{
    PrioritySet* tmp = this;
    while (tmp != nullptr)
    {
        if (tmp->val == value)
        {
            tmp->next = tmp->next->next;
            return true;
        }
    }
    return false;
}

bool PrioritySet::has(int value)
{
    PrioritySet* tmp = this;
    while (tmp != nullptr)
    {
        if (tmp->val == value)
        {
            return true;
        }
    }
    return false;
}

