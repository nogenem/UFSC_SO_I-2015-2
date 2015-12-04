/*
 * File:   Queue.h
 * Author: <preencher>
 *
 * Created on September 27, 2015, 11:28 AM
 */

#ifndef QUEUE_H
#define	QUEUE_H

#include <list>

/**
 * Used by Thread as waiting queues for threads that uses void Thread::sleep(Queue* e) and void Thread::wakeup(Queue q)
 **/

// INSERT YOUR CODE HERE ; CHANGE THE QUEUE THE WAY YOU WANT

template <class T>
class Queue : public std::list<T> {
public: // constructors
    Queue() : std::list<T>() {};
    Queue(const Queue& orig) : std::list<T>(orig){};
    virtual ~Queue(){};
    
    // preferi usar o código do professor...
    void push(T value){ this->push_back(value); }
    
    void pop(){ 
        if (!this->empty()) {
            this->erase(this->begin());
        } 
    }
    
    T top(){ return (*this->begin()); }
private: // user defined
    // INSERT YOUR CODE HERE
    // ...
};

#endif	/* QUEUE_H */

