// -----------------------------------------------------------------------------
//             __            _         _            _     
//            / _|          | |       | |          | |    
//  _ __  ___| |_  ___ _ __ | |_ _ __ | |_ _ __    | |__  
// | '__|/ _ \  _|/ __| '_ \| __| '_ \| __| '__|   | '_ \ 
// | |  |  __/ | | (__| | | | |_| |_) | |_| |    _ | | | |
// |_|   \___|_|  \___|_| |_|\__| .__/ \__|_|   (_)|_| |_|
//                              | |                       
//                              |_|                       
//
// Reference Counting Pointer
//
// -----------------------------------------------------------------------------
// Originally created on 04/06/2003 by Travis McLane
//
// Copyright 2003, Procyon Software, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_REFCNTPTR
#define _H_REFCNTPTR

// -----------------------------------------------------------------------------

template <class T>
class RefCntPtr
{
	// Construction/Destruction
	public:
	    explicit RefCntPtr(T* realPtr = 0){
            Acquire(new Surrogate);
            surrogate->pointee = realPtr;
        };
		
		RefCntPtr(RefCntPtr<T>& rhs){
			Acquire(rhs.surrogate);
		};
		
	    ~RefCntPtr(){ Release(); };

	// Operators
        T* operator->() const
		{
			return surrogate->pointee; 	
		};

		T& operator*() const
		{
		    return *surrogate->pointee;
		}; 

        
        //RefCntPtr<T>& operator=( RefCntPtr<T>& );
		RefCntPtr<T>& operator=( RefCntPtr<T>& rhs)
		{
			if ( surrogate )
				Release();
		    
			Acquire(rhs.surrogate);
		    
			return *this;
		};
		RefCntPtr<T>& operator=(const RefCntPtr<T>& rhs)
		{
			if ( surrogate )
				Release();
		    
			Acquire(rhs.surrogate);
		    
			return *this;
		};
		//RefCntPtr<T>& operator=( T& );
		//RefCntPtr<T>& operator=( T* );
		RefCntPtr<T>& operator=( T* newT )
		{
			if ( surrogate )
				Release();
		        
			Acquire(new Surrogate);
		    
			surrogate->pointee = newT;

			return *this;
		};

	// Implementation

	// Accessors
        bool Unique()
        {
            return surrogate->count == 1;
        };
        
	private:
      	// Data members
        struct Surrogate{
	       int count;
	       T*  pointee;
	    } *surrogate;        
          
        void Acquire(Surrogate* sur)
        {
            surrogate = sur;
            if ( sur ) 
				++sur->count;
        };
        
        void Release()
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
};

#endif // _H_REFCNTPTR
// -----------------------------------------------------------------------------
// refcntptr.h - End of file
// -----------------------------------------------------------------------------

