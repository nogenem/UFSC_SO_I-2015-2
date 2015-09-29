/*
 * File:   MemoryManager.h
 * Authors: INSERT YOUR NAME HERE
 * Strategy: Partições dinâmicas - Alg. Best-Fit
 *
 * Created on ...
 */

#ifndef MEMORYMANAGER_H
#define	MEMORYMANAGER_H

#include <list>
#include <vector>
#include "Process.h"

class Partition {
public:
    Partition(){}
    Partition(Process* process, unsigned int beginAddress, unsigned int endAddress);
    Partition(unsigned int beginAddress, unsigned int endAddress);

    Process* getProcess(){
        return _process;
    }
    void setBeginAddress(unsigned int beginAddress){
        this->_beginAddress = beginAddress;
    }
    void setEndAddress(unsigned int endAddress){
        this->_endAddress = endAddress;
    }
public:  // do not change
    unsigned int getBeginAddress() {
        return _beginAddress;
    }
    unsigned int getEndAddress() {
        return _endAddress;
    }
    unsigned int getLength() {
        return _endAddress - _beginAddress + 1;
    }
private: // do not change
        unsigned int _beginAddress;
        unsigned int _endAddress;
private:
    Process* _process;
};

class MemoryManager {
public: // do not change
    MemoryManager(unsigned int size);
    MemoryManager(const MemoryManager& orig);
    virtual ~MemoryManager();
public: // do not change
    void allocateMemoryForProcess(Process* process);
    void deallocateMemoryOfProcess(Process* process);
    void showAllocatedMemory();
    unsigned int getNumPartitions();
    Partition* getPartition(unsigned int index);
    unsigned int getSize() const;
private: // private attributes
    unsigned int _size;

    // Lista de partições ocupadas
    std::list<Partition*> _busyList;
    // Lista de espaços livres na memória
    std::list<Partition*> _freeList;
    // Fila de processos para serem alocados
    std::vector<Process*> _queue;
private: // private methods
    unsigned int getProcessSize(Process* process);
    void addPartitionInOrder(Partition* p, bool addOnBusy=true);
};

#endif	/* MEMORYMANAGER_H */

