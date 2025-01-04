// -----------------------------------------------------------------------------
//             __            _         _                             
//            / _|          | |       | |                            
//  _ __  ___| |_  ___ _ __ | |_ _ __ | |_ _ __      ___ _ __  _ __  
// | '__|/ _ \  _|/ __| '_ \| __| '_ \| __| '__|    / __| '_ \| '_ \ 
// | |  |  __/ | | (__| | | | |_| |_) | |_| |    _ | (__| |_) | |_) |
// |_|   \___|_|  \___|_| |_|\__| .__/ \__|_|   (_) \___| .__/| .__/ 
//                              | |                     | |   | |    
//                              |_|                     |_|   |_|    
//
// Reference Counting Pointer
//
// -----------------------------------------------------------------------------
// Originally created on 04/06/2003 by Travis McLane
//
// Copyright 2003, Procyon Software, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "refcntptr.h"

// -----------------------------------------------------------------------------
/*
template <class T>
RefCntPtr<T>::RefCntPtr(T* realPtr = 0)
{
    Acquire(new Surrogate);
    surrogate->pointee = realPtr;
};

// -----------------------------------------------------------------------------

template <class T>
RefCntPtr<T>::~RefCntPtr()
{
    Release();
};


// -----------------------------------------------------------------------------

template <class T>
T* RefCntPtr<T>::operator->() const 
{
    return surrogate->pointee; 
};
// -----------------------------------------------------------------------------

template <class T>        
T& RefCntPtr<T>::operator*() const
{
    return *surrogate->pointee;
};

// -----------------------------------------------------------------------------
/*
template <class T>        
RefCntPtr<T>& RefCntPtr<T>::operator=( RefCntPtr<T>& rhs)
{
    if ( surrogate )
        Release();
    
    Acquire(rhs.c);
    
    return *this;
};


// -----------------------------------------------------------------------------

template <class T>        
RefCntPtr<T>& RefCntPtr<T>::operator=( T* newT )
{
    if ( surrogate )
        Release();
        
    Acquire(new Surrogate);
    
    surrogate->pointee = newT;
};

*/
// -----------------------------------------------------------------------------
/*
template <class T>        
RefCntPtr<T>& RefCntPtr<T>::operator=( T& newT )
{
    if ( surrogate )
        Release();
        
    Acquire(new Surrogate);
    
    surrogate->pointee = newT;
};
*/

// -----------------------------------------------------------------------------

/*
template <class T>
bool RefCntPtr<T>::Unique()
{
    return (surrogate->count == 1);
};
*/
// -----------------------------------------------------------------------------
/*
template <class T>
void RefCntPtr<T>::Release()
{
    if ( surrogate )
    {
        if (--surrogate->count == 0)
        {
            delete surrogate->pointee;
            delete surrogate;
        }
        surrogate = 0;
    }        
};

// -----------------------------------------------------------------------------

template <class T>
void RefCntPtr<T>::Acquire(Surrogate *c)
{
    surrogate = c;
    if ( c )
        ++c->count;
};
	*/
// -----------------------------------------------------------------------------
// refcntptr.cpp - End of file
// -----------------------------------------------------------------------------
