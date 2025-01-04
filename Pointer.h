#ifndef POINTER_H
#define POINTER_H

template class <T>
class RefCountPtr <T>
{
    public:
        RefCountPtr();
        ~RefCountPtr();
    
        T operator*()
        void operator delete(){
            count--;
            if ( count == 0 )
                delete m_ptr;
        };
        
    private:
        int count;
        T m_ptr;
};

#endif