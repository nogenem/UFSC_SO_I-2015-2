/*
 * File:   MemoryManager.cpp
 * Authors: INSERT YOUR NAME HERE
 * Strategy: Partições dinâmicas - Alg. Best-Fit
 *
 * Created on ...
 */

#include <iostream>
#include "MemoryManager.h"
#include "Simulator.h"

Partition::Partition(Process* process, unsigned int beginAddress, unsigned int endAddress):
    _beginAddress(beginAddress), _endAddress(endAddress), _process(process){}
Partition::Partition(unsigned int beginAddress, unsigned int endAddress):
    Partition(nullptr, beginAddress, endAddress){}

MemoryManager::MemoryManager(unsigned int size) {
    // do not change
    this->_size = size;

    // No começo toda a memória esta livre
    _freeList.push_back(new Partition(0, _size-1));
}
MemoryManager::MemoryManager(const MemoryManager& orig):
    MemoryManager(orig.getSize()){}

MemoryManager::~MemoryManager() {
    for(auto it = _busyList.begin(); it != _busyList.end(); it++){
        delete (*it);
    }
    for(auto it = _freeList.begin(); it != _freeList.end(); it++){
        delete (*it);
    }
}

/**
 * Retorna o tamanho de um processo, somando o tamanho de seus
 *  segmentos.
 *
 * @param Process* process  => Processo que se quer saber o tamanho
 */
unsigned int MemoryManager::getProcessSize(Process* process){
    auto segments = process->getSegments();
    unsigned int size = 0;
    for(auto it = segments->begin(); it != segments->end(); it++)
        size += (*it)->getSize();

    return size;
}

/**
 * Adiciona uma partição em uma das listas do sistema, dependendo
 *  da variavel addOnBusy, em ordem de endereçamento.
 *
 * @param Partition* p     => Partição a ser adicionada
 * @param bool addOnBusy   => TRUE se quiser adicionar a partição na _busyList
 *                            FALSE se quiser adicionar na _freeList
 */
void MemoryManager::addPartitionInOrder(Partition* p, bool addOnBusy){
    auto list = (addOnBusy ? &_busyList : &_freeList);

    // Acha a partição na lista com endereço inicial maior que
    //  o endereço final da partição a ser adicionada.
    bool found = false;
    for(auto it = list->begin(); it != list->end(); it++){
        auto tmp = (*it);
        if(tmp->getBeginAddress() > p->getEndAddress()){
            list->insert(it, p);// Adiciona atras da posição passada, it
            found = true;
            break;
        }
    }

    // Se não achou nenhuma partição com endeço maior,
    //  então adiciona ao final da lista
    if(!found)
        list->push_back(p);
}

void MemoryManager::allocateMemoryForProcess(Process* process) {
    unsigned int process_size = getProcessSize(process);

    // Alg. Best-fit descrito no livro [4]
    unsigned int initial_memory_waste = this->_size;
    unsigned int memory_waste = 0;
    Partition *best = nullptr;
    auto itBest = _freeList.end();

    for(auto it = _freeList.begin(); it != _freeList.end(); it++){
        if(process_size <= (*it)->getLength())
            memory_waste = (*it)->getLength() - process_size;
        else
            continue;

        if(initial_memory_waste > memory_waste){
            best = (*it);
            itBest = it;
            initial_memory_waste = memory_waste;
        }
    }

    if(best != nullptr){// Achou uma partição livre que caiba o processo
        unsigned int begin = best->getBeginAddress();
        unsigned int end = begin + process_size - 1;

        // Atualiza os limites de memória do processo
        process->setBeginMemory(begin);
        process->setEndMemory(end);

        // Atualiza a _freeList
        if(initial_memory_waste != 0){
            // Caso o processo não caiba perfeitamente na partição,
            //  então deve-se atualizar o endereço inicial da partição
            best->setBeginAddress(end+1);
        }else{
            // Caso o processo caiba perfeitamente na partição,
            //  então deleta ela da _freeList
            delete best;
            _freeList.erase(itBest);
        }

        // Adiciona a nova partição na _busyList
        addPartitionInOrder(new Partition(process, begin, end), true);

        Simulator::getInstance()->getModel()->getProcessManager()->scheduler_choose(process);
    }else{// Processo entrou na fila de espera
        _queue.push_back(process);
    }
}

void MemoryManager::deallocateMemoryOfProcess(Process* process) {

    // Pequena proteção...
    if(process->getBeginMemory() == 0 &&
        process->getEndMemory() == 0)
        return;

    // Verifica se tem partições adjacentes
    auto itUp = _freeList.end();// Iterador da partição acima
    auto itDown = _freeList.end();// Iterador da partição abaixo
    for(auto it = _freeList.begin(); it != _freeList.end(); it++){
        auto tmp = (*it);
        if(tmp->getEndAddress()+1 == process->getBeginMemory()){
            itUp = it;
        }else if(tmp->getBeginAddress()-1 == process->getEndMemory()){
            itDown = it;
        }

        if(tmp->getBeginAddress() > process->getEndMemory())
            break;
    }

    auto pUp = (*itUp);// Partição acima
    auto pDown = (*itDown);// Partição abaixo

    // Espaço que ficará livre após este desalocamento
    unsigned int freeSize = 0;

    if(itUp != _freeList.end() && itDown != _freeList.end()){// Acima e abaixo
        // Existem partições livres acima e abaixo da partição que será desalocado

        pUp->setEndAddress(pDown->getEndAddress());// Atualiza o endereço final da partição acima
                                                   //  com o endereço final da partição abaixo [junta elas]
        freeSize = pUp->getLength();
        delete pDown;
        _freeList.erase(itDown);// Remove a partição abaixo
    }else if(itUp != _freeList.end() && itDown == _freeList.end()){// Só acima
        // Existe uma partição livre acima da que será desalocada

        pUp->setEndAddress(process->getEndMemory());// Atualiza o endereço final da partição acima
                                                    //  com o endereço final da partição que será desalocada
        freeSize = pUp->getLength();
    }else if(itUp == _freeList.end() && itDown != _freeList.end()){// Só abaixo
        // Existe uma partição livre abaixo da que será desalocada

        pDown->setBeginAddress(process->getBeginMemory());// Atualiza o endereço inicial da partição abaixo
                                                          //  com o endereço inicial da partição que será
                                                          //  desalocada
        freeSize = pDown->getLength();
    }else{// Isolada
        // A partição esta isolada, então simplesmente adicione
        //  ela na _freeList.
        auto tmp = new Partition(process->getBeginMemory(),
                                    process->getEndMemory());
        addPartitionInOrder(tmp, false);
        freeSize = tmp->getLength();
    }

    // Remove a partição do processo da _busyList
    for(auto it = _busyList.begin(); it != _busyList.end(); it++){
        auto tmp = (*it);
        if(tmp->getProcess() == process){
            delete (*it);
            _busyList.erase(it);
            break;
        }
    }

    // Zera a memória do processo
    process->setBeginMemory(0);
    process->setEndMemory(0);

    // Verifica se tem algum processo na fila que
    //  caiba no espaço gerado por esta desalocação
    unsigned int pSize = 0;
    for(auto it = _queue.begin(); it != _queue.end(); it++){
        auto tmp = (*it);
        pSize = getProcessSize(tmp);
        if(freeSize >= pSize){
            allocateMemoryForProcess(tmp);
            _queue.erase(it);
            break;
        }
    }
}

unsigned int MemoryManager::getNumPartitions() {
    // INSERT YOUR CODE TO RETURN THE QUANTITY OF ALLOCATED PARTITIONS

    return _busyList.size();
}

Partition* MemoryManager::getPartition(unsigned int index) {
    // INSERT YOUR CODE TO RETURN THE PARTITION AT INDEX index

    auto it = _busyList.begin();
    std::advance(it, index);
    return (*it);
}

void MemoryManager::showAllocatedMemory() {
    unsigned int beginAllocatedAddress, endAllocatedAddress, totalAllocated, processId, internalFragmentation, externalFragmentation;
    // INSERT YOUR CODE TO SHOW EACH ONE OF THE ALLOCATED MEMORY PARTITIONS, INCLUDING INTERNAL AND EXTERNAL (THE FOLLOWING) FRAGMENTATION

    auto freeIt = _freeList.begin();
    for(auto it = _busyList.begin(); it != _busyList.end(); it++){  // for each partition...

        auto p = (*it);

        beginAllocatedAddress = p->getBeginAddress();
        endAllocatedAddress = p->getEndAddress();
        totalAllocated = p->getLength();
        processId = p->getProcess()->getId();
        internalFragmentation = 0;

        externalFragmentation = 0;
        // Como as listas estão ordenadas por endereço,
        //  só é preciso verificar a _freeList em ordem
        if(p->getBeginAddress() > (*freeIt)->getEndAddress())
            freeIt++;

        if((*freeIt)->getBeginAddress() == p->getEndAddress()+1)
            externalFragmentation = (*freeIt)->getLength();


        // no not change the next line (the way information are shown)
        std::cout << "\tAllocd: " << "B=" << (beginAllocatedAddress) << ", \tE=" << (endAllocatedAddress) << ", \tT=" << (totalAllocated) << ", \tPID=" << (processId)
                << ", \tIF=" << (internalFragmentation) << ", \tEF=" << (externalFragmentation) << "\n";
    }
}

unsigned int MemoryManager::getSize() const {
    return _size;
}
