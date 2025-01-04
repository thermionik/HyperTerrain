#include "quadstack.h"
#include "quadtritree.h"

QuadStack::QuadStack() : head(NULL), tail(NULL), size(0) {};

QuadStack::~QuadStack(){
        DeleteAll();
};

int QuadStack::Size(){
    return size;
};

bool QuadStack::IsEmpty(){
    return !head;
};

void QuadStack::DeleteAll(){
//    QuadTriNodePtr temp;
    while (!IsEmpty())
        pop();
};

void QuadStack::UnlinkNode(QuadStackNode* qnode){
    if (qnode)
    {   
        if ( qnode == head )
        {
            if ( Size() == 1 )
                head = tail = NULL;
            else
            {
                head = head->next;
                head->prev = NULL;
            }
                
            size--;
        }
        else
        if ( qnode == tail )
        {
            if ( Size() == 1 )
                head = tail = NULL;
            else
            {
                tail = tail->prev;
                tail->next = NULL;
            }
             
            size--;
        }
        else
        { // somewhere in the sprawling metropolis
            qnode->prev->next = qnode->next;
            qnode->next->prev = qnode->prev;
            size--;
        }
        
        qnode->next = NULL;
        qnode->prev = NULL;
        qnode->diamond->queue = NULL;
        qnode->diamond->queueloc = NULL;
        qnode->diamond = NULL;
        
//        delete qnode;
    }
};

QuadStackNode* QuadStack::begin(){
    return head;
};

QuadStackNode* QuadStack::end(){
    return tail;
};

QuadStackNode* QuadStack::push(QuadTriTree* qtnode){ //Returns address of QueueNode if successful
    QuadStackNode* newNode;
    newNode = new QuadStackNode;
    newNode->diamond = qtnode;
    newNode->prev = NULL;
           
    if ( head )
    {
        newNode->next = head;
        newNode->next->prev = newNode;
        head = newNode;
    }
    else
    {
        head = tail = newNode;
        newNode->next = NULL;
    }
    newNode->diamond->queueloc = newNode;
    newNode->diamond->queue = this;
    
    size++;
    return head;
};  

QuadTriTree* QuadStack::pop(){
    QuadStackNode* temp;
    QuadTriTree* diamond;
    
    if (head == tail){ //Only one element in stack
        temp = head;
        head = tail = NULL;
        diamond = temp->diamond;
        temp->diamond = NULL;
        temp->next = temp->prev = NULL;
        //delete temp;
    }
    else{ //More than one element in stack
        temp = head;
        head = head->next;
        head->prev = NULL; //Set new head element
        temp->next = NULL;
        diamond = temp->diamond;
        temp->diamond = NULL;
        //delete temp;
    }
    diamond->queue = NULL;
    diamond->queueloc = NULL;

    size--;
    return diamond;
};