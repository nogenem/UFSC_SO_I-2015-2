/*
 * File:   Thread.h
 * Author: <preencher>
 *
 * Created on September 27, 2015, 10:30 AM
 */

#ifndef THREAD_H
#define	THREAD_H

#include "Queue.h"
#include "Process.h"
#include "TimerMediator.h"
#include "ModuleInvoke_ThreadExec.h"

class Thread {
    friend class ModuleInvoke_ThreadExec; // simulator use only
    friend class TimerMediator;
    friend class Process;

    enum State {
        RUNNING,
        READY,
        WAITING,
        FINISHING
    };

    enum Criterion {
        HIGH,
        NORMAL,
        LOW,
        MAIN,
        IDLE
    };

    /**
     *  struct for grouping threads' attributes
     **/
    struct AccountInformation {
        // do not change
        int _priority=0;        // this attribute MUST BE the one used to sort the Schelung_Queue, no matter the scheduler algorithm
        double _arrivalTime=0;  // tempo que a thread chegou no sistema
        double _cpuBurstTime=0; // estimativa de tempo que a thread vai utilizar a CPU
        double _cpuTime=0;      // esta sendo usado como uma variável temporária para calcular o próximo 'chute' de cpuBurstTime
        double _waitingTime=0;  // não esta sendo usado para nada...
        
        // add here another information you need
        // INSERT YOUR CODE HERE
        
        double _startTime=0;    // tempo que o processo pegou a CPU
    };

public: // constructors
    Thread(Process* task, Thread::State state = Thread::State::READY);
    Thread(const Thread& orig);
    virtual ~Thread();
public: // statics
    static void sleep(Queue<Thread*> * q);
    static void wakeup(Queue<Thread*> * q);
    static Thread* thread_create(Process* parent);

    static std::list<Thread*>* getThreadsList() {
        static std::list<Thread*>* _threadsList;
        if (_threadsList == nullptr) {
            _threadsList = new std::list<Thread*>();
        }
        return _threadsList;
    }
protected: // statics
    static Thread* _running;
    static void yield();
    static void exit(int status = 0);
    static Thread* running();
    static void dispatch(Thread* previous, Thread* next);

    static unsigned int getNewId() {
        static unsigned int _lastId = 0;
        return ++_lastId;
    }
public: //
    int join();
    Process* getProcess();
    int getPriority() const;
protected:
    Process * _process=nullptr;
    State _state;
    unsigned int _id;
    AccountInformation _accountInfo;
    Queue<Thread*>* _queue=nullptr;

public: // USER DEFINED
    // INSERT YOUR CODE HERE
    
    void unlockWaitingList();
    void removeFromWaitingList(Thread *t);
private: // USER DEFINED
    // INSERT YOUR CODE HERE
    
    void _updateNextGuess();
    void _updateRemainTime();
    void _unlockWaitingList(bool finishing);
};

#endif	/* THREAD_H */

