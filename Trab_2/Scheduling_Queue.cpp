/*
 * File:   Scheduling_Queue.cpp
 * Author: cancian
 *
 * Created on October 9, 2015, 9:34 PM
 */

#include "Scheduling_Queue.h"

Scheduling_Queue::Scheduling_Queue() {
    // CHANGE AS NECESSARY IF YOU CHOOSE ANOTHER QUEUE
    _queue = new std::list<Thread*>();
}

Scheduling_Queue::Scheduling_Queue(const Scheduling_Queue& orig) {
}

Scheduling_Queue::~Scheduling_Queue() {
}

bool Scheduling_Queue::empty() {
    // CHANGE AS NECESSARY IF YOU CHOOSE ANOTHER QUEUE
    return _queue->empty();
}

void Scheduling_Queue::pop() {
    // CHANGE AS NECESSARY IF YOU CHOOSE ANOTHER QUEUE
    if (!_queue->empty()) {
        _queue->erase(_queue->begin());
    }
}

void Scheduling_Queue::remove(Thread* t) {
    _queue->remove(t);
}

Thread* Scheduling_Queue::top() {
    // CHANGE AS NECESSARY IF YOU CHOOSE ANOTHER QUEUE
    return (*_queue->begin());
}

// função de comparação usada pelo list::sort
bool comp(Thread* t1, Thread* t2){
    return t1->getPriority() < t2->getPriority();
}

void Scheduling_Queue::insert(Thread* t) {
    // INSERT YOUR CODE HERE
    
    this->_queue->push_back(t);
    this->_queue->sort(comp);
    
    /*
    // Adiciona ja em ordem, percorrendo a lista e adicionando
    //  a nova thread antes da primeira thread com prioridade
    //  maior que ela
    bool found = false;
    for(auto it = _queue->begin(); it != _queue->end(); it++){
        auto &tmp = (*it);
        if(tmp->getPriority() > t->getPriority()){
            _queue->insert(it, t);
            found = true;
            break;
        }
    }

    // Se a thread tiver a pior prioridade,
    //  então adiciona ao final da lista
    if(!found)
        _queue->push_back(t);
    */
}
