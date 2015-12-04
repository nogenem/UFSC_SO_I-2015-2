/*
 * File:   Traits.h
 * Author: cancian
 *
 * Created on September 27, 2015, 4:16 PM
 */

#ifndef TRAITS_H
#define	TRAITS_H

#include "Debug.h"
#include "CPU.h"
#include "Thread.h"
#include "Model.h"
#include "Scheduler.h"
#include "Process.h"

template<typename T>
struct Traits {
    static const bool enabled = true;
    static const bool debugged = true;
};

template<> struct Traits<Debug> { // CHANGE THE DEBUG LEVEL HERE SETTING THE LEVELS YOU WANT TO SHOW
    static const bool error = true;
    static const bool warning = true;
    static const bool trace = false; //false;
    static const bool info = false; //false;
    static const bool fine = false;
};

template<> struct Traits<CPU> {
    static constexpr double context_switch_overhead = 1.0; // time units
    static constexpr double timer_interrupt_period = 100.0; // time units
};

template<> struct Traits<Process> {
    static constexpr double timeBetweenCresations = 1000.0; // time units
};

template<> struct Traits<Thread> {
    static constexpr double minCpuBurst = 150.0; // time units
    static constexpr double maxCpuBurst = 150.0; // time units
    static constexpr int minThreadsPerProcess = 3;
    static constexpr int maxThreadsPerProcess = 3;
};

template<> struct Traits<Model> {
    static constexpr double simulationLength = 4000;//3950.0; // time units
    static constexpr double firstCreation = 10.0; // time units
};

template<> struct Traits<MemoryManager> {
    static constexpr unsigned int memorySize = 1e6;
};

template<> struct Traits<Scheduler> {
    static constexpr double timeSlice = 200.0; // time units
    // INSERT YOUR CODE HERE
    // ...
};


#endif	/* TRAITS_H */

