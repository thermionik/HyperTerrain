#ifndef QUADSTACK_H
#define QUADSTACK_H

#include "quadtritree.h"

struct QuadStackNode{
    QuadStackNode* next;
    QuadStackNode* prev;
    QuadTriTree* diamond;
};

class QuadStack{
    public:
        QuadStack();
        ~QuadStack();
        void UnlinkNode(QuadStackNode*);
        QuadStackNode* push(QuadTriTree*);  //Returns address of QueueNode if successful
        QuadTriTree* pop();
        bool IsEmpty();
        void DeleteAll();
        QuadStackNode* begin();
        QuadStackNode* end();
        int Size();
#ifndef WIN32
    friend class Iterator{
#else
		class Iterator{
#endif
        public:
                Iterator() : current(NULL) {};
                ~Iterator(){ current = NULL; };
                QuadStackNode* operator ++(int){ current = current->next; return current; };
                QuadStackNode* operator --(int){ current = current->prev; return current; };
                void operator= (QuadStackNode* toPoint){ current = toPoint; };
                bool operator!= (QuadStackNode* compare) { return compare != current; }; 
                QuadStackNode* operator->() { return current; };
                QuadTriTree* operator*() { return current->diamond; };
                
                QuadStackNode* current;
    };
            
    protected:
        QuadStackNode* head;
        QuadStackNode* tail;
        int size;
};

#endif        