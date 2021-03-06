#pragma once

#include <vector>

/*! \file
* Utility functions for vectors
*/

//! Check if an item is in a vector
template <class T>
bool itemInVector(T item, const std::vector<T> *vec)
{
    for ( T element : *vec )
    {
        if ( element == item ) { return true; }
    }
    return false;
}

/*! \brief removes all instances of a specified item from
*         a vector
*         if item(s) were removed, returns true
*/
template<class T>
bool removeItemFromVector(T item, std::vector<T> *vec)
{
    std::vector<int> toRemove;
    for ( int i = 0; i < vec->size(); i++ )
    {
        T element = vec->at(i);
        if ( element == item ) { toRemove.push_back(i); }
    }
    
    for (int pos : toRemove) { vec->erase(vec->begin()+pos); }
    return toRemove.size() > 0;
}