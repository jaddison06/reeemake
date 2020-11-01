#include "VectorThings.h"

template <class T>
bool itemInVector(T item, std::vector<T> *vec)
{
    for ( T element : *vec )
    {
        if ( element == item ) { return true; }
    }
    return false;
}

// removes all instances of a specified item from
// a vector
//
// if item(s) were removed, returns true
template<class T>
bool removeItemFromVector(T item, std::vector<T> *vec)
{
    std::vector<int> toRemove;
    for ( int i = 0; i < vec->size(); i++ )
    {
        T element = vec->at(i);
        if ( element == item ) { toRemove.push_back(i); }
    }
    
    for (int pos : toRemove) { vec->erase(pos); }
    return toRemove.size() > 0;
}