/*
 * File:   Scheduling_Queue.h
 * Author: cacian
 *
 * Created on October 9, 2015, 9:34 PM
 */

#ifndef SCHEDULING_QUEUE_H
#define	SCHEDULING_QUEUE_H

#include <list>

#include "Thread.h"

/**
 * Used by Scheduler to store Threads in the RUNNING state. This is the class for the READY_QUEUE
 */
class Scheduling_Queue /* insert hierarchy if you want */ {
public:
    Scheduling_Queue();
    Scheduling_Queue(const Scheduling_Queue& orig);
    virtual ~Scheduling_Queue();
public:
    bool empty();
    void insert(Thread* t);
    void remove(Thread* t);
    void pop(); // remove the top (head) element
    Thread* top(); // returns the top (head) element
private:
    // INSERT YOUR CODE HERE
    // ...
    std::list<Thread*>* _queue; /* change or delete if you want. It is just a suggestion */
};

#endif	/* SCHEDULING_QUEUE_H */

