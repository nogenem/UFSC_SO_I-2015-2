/*
 * File:   Thread.cpp
 * Author: <preencher>
 *
 * Created on September 27, 2015, 10:30 AM
 */

#include "Thread.h"
#include "Debug.h"
#include "System.h"
#include "Simulator.h"
#include "CPU.h"

#include <iostream>

Thread* Thread::_running=nullptr;

Thread::Thread(Process* task, Thread::State state) {
    this->_id = Thread::getNewId();
    this->_process = task;
    this->_state = state;
    this->_queue = new Queue<Thread*>();
    // INSERT YOUR CODE HERE
   
    // tempo de chegada ao sistema
    this->_accountInfo._arrivalTime = Simulator::getInstance()->getTnow();
}

Thread::Thread(const Thread& orig) {
}

Thread::~Thread() {
    _unlockWaitingList(true);
}

Thread* Thread::running() {
    return _running;
    //return System::scheduler()->choosen();
}

Process* Thread::getProcess() {
    return _process;
}

int Thread::getPriority() const {
    return _accountInfo._priority;
}

/**
 * 'Destrava' as threads esperando por esta thread, colocando-as no 
 *  estado READY, adicionando-as a lista do escalonador e 
 *  removendo-as da lista de espera desta thread.
 * 
 * @param bool deleting         TRUE se a thread estiver sendo destruida,
 *                              FALSE caso contrario.
 **/
void Thread::unlockWaitingList(){
    this->_unlockWaitingList(false);
}

void Thread::_unlockWaitingList(bool deleting){
    while(!this->_queue->empty()){
        Thread *tmp = this->_queue->top();
        
        // Caso a thread esteja sendo destruida, não é preciso por
        //  as threads do mesmo processo na lista de READY, pois o processo
        //  pai delas esta sendo destruido
        if(!deleting || (deleting && tmp->_process != this->_process)){
            // todas as threads na lista devem ser colocadas no estado READY
            tmp->_state = Thread::State::READY;
            // e colocadas na fila de threads prontas para executar
            System::scheduler()->insert(tmp);
        }
        
        this->_queue->pop();
    }
}

/**
 * Remove uma thread especifica da lista de threads esperando
 *  por esta thread.
 *
 * @param Thread* t         Thread a ser removida da lista de espera.
 **/
void Thread::removeFromWaitingList(Thread *t){
    this->_queue->remove(t);
}

/**
 * Atualiza a prioridade desta thread para o tempo restante 
 *  de utilização de CPU dela.
 **/
void Thread::_updateRemainTime(){
        
    double t = Simulator::getInstance()->getTnow() - this->_accountInfo._startTime;
    
    // pequena proteção... pode ocorrer caso aconteça alguma coisa 
    //  antes do tempo de troca de contexto 
    if(t < 0.0){ t = 0.0; }
    
    // para se proteger de prioridades negativas
    if(this->_accountInfo._priority < 0)
        this->_accountInfo._priority += t;
    else{
        this->_accountInfo._priority -= t;
        
        // caso uma thread fique na CPU mais tempo do que era previsto...
        if(this->_accountInfo._priority < 0){
            _updateNextGuess(); // esta certo? 
        }
    }
    this->_accountInfo._cpuTime += t;
}

/**
 * Atualiza a prioridade e o cpuBurstTime seguinte desta thread usando
 *  o tempo que esta thread ficou na cpu e o ultimo 'chute'.
 **/
void Thread::_updateNextGuess(){
    
    // calcula o proximo 'chute' para o cpuBurstTime e seta a prioridade
    double t  = Simulator::getInstance()->getTnow() - this->_accountInfo._startTime;
           t += this->_accountInfo._cpuTime;
    
    // pequena proteção... pode ocorrer caso ocorra alguma coisa 
    //  antes do tempo de troca de contexto
    if(t < 0.0){ t = 0.0; }
    
    double lastGuess = this->_accountInfo._cpuBurstTime;
    double newGuess = 0.5*t + 0.5*lastGuess;
    
    this->_accountInfo._cpuBurstTime = newGuess;
    // prioridade de uma thread é o seu cpuBurstTime truncado
    this->_accountInfo._priority = newGuess;
    this->_accountInfo._cpuTime = 0;
}

void Thread::sleep(Queue<Thread*>* q) {
    Debug::cout(Debug::Level::trace, "Thread::sleep(" + std::to_string(reinterpret_cast<unsigned long> (q)) + ")");
    // INSERT YOUR CODE HERE
    // ...
}

void Thread::wakeup(Queue<Thread*>* q) {
    Debug::cout(Debug::Level::trace, "Thread::wakeup(" + std::to_string(reinterpret_cast<unsigned long> (q)) + ")");
    // INSERT YOUR CODE HERE
    // ...
}

/**
 * Threads podem decidir deixar a CPU invocando a chamada void Thread::yield(), que precisa ser implementada. A implementação
 * dessa chamada deve colocar a thread que está executando no estado READY, incluí-la na fila de threads prontas, chamar
 * o escalonador para escolher outra thread, invocando Thread* Scheduler::choose() e então o despachador para iniciar
 * a execução da thread escolhida pelo escalonador, invocando static void Thread::dispatch(Thread* previous, Thread* next).
 **/
void Thread::yield() {
    Debug::cout(Debug::Level::trace, "Thread::yield()");
    // INSERT YOUR CODE HERE
    
    Thread *running = Thread::running();
    
    // só para garantir... 
    if(running == nullptr)
        return;
    
    // colocar a thread que está executando no estado READY
    running->_state = Thread::State::READY;  
    
    // atualiza a prioridade da thread
    running->_updateNextGuess();
    
    // incluí-la na fila de threads prontas
    System::scheduler()->insert(running);
    
    // chamar o escalonador para escolher outra thread
    Thread *next = System::scheduler()->choose();
    
    // e então o despachador para iniciar a execução da thread escolhida pelo escalonador
    Thread::dispatch(running, next);
}

/**
 * Threads aguardarão pelo término de outras threads quando for invocada a chamada void Thread::join(), que precisa ser 
 * implementada. A implementação dessa chamada deve verificar se a thread this passada como parâmetro está no estado FINISHING 
 * ou não. Se estiver, então a thread que invocou a chamada (a que está executando --running--) simplesmente continua sua execução. 
 * Se não estiver, então a thread executando deve ser colocada no estado WAITING e colocada na fila de threads esperando pela 
 * thread this. Ainda nesse caso, deve-se chamar o escalonador para escolher outra thread, invocando Thread* Scheduler::choose() 
 * e então o despachador para iniciar a execução da thread escolhida pelo escalonador, invocando 
 * static void Thread::dispatch(Thread* previous, Thread* next). Para melhor ilustrar essa chamada, se o código da thread T1 
 * tiver a chamada T2->join(), então T1 é quem está executando (running) e T2 é quem foi invocada (this), e é T1 que deve ser 
 * bloqueada esperando por T2 (se T2 não estiver FINISHING).
 **/
int Thread::join() {
    Debug::cout(Debug::Level::trace, "Thread::join()");
    // INSERT YOUR CODE HERE
    
    if(this->_state != Thread::State::FINISHING){
        Thread *running = Thread::running();
        
        // só para garantir... 
        if(running == nullptr)
            return 0;
        
        // thread executando deve ser colocada no estado WAITING
        running->_state = Thread::State::WAITING;
        
        // atualiza a prioridade da thread
        running->_updateNextGuess();
        
        // colocada na fila de threads esperando pela thread this
        this->_queue->push(running);
        
        // chamar o escalonador para escolher outra thread
        Thread *next = System::scheduler()->choose();
        
        // e então o despachador para iniciar a execução da thread escolhida pelo escalonador
        Thread::dispatch(running, next);
    }
    
    return 0;
}

/**
 * Threads serão finalizadas quando for invocada a chamada void Thread::exit(), que precisa ser implementada. A implementação
 * dessa chamada deve colocar a thread que está executando no estado FINISHED, verificar se há alguma thread na lista
 * de threads bloqueadas esperando por essa thread. Se houver, todas as threads na lista devem ser colocadas no estado
 * READY e colocadas na fila de threads pronas para executar. Em qualquer caso, deve-se ainda chamar o escalonador para
 * escolher outra thread, invocando Thread* Scheduler::choose() e então o despachador para iniciar a execução da thread
 * escolhida pelo escalonador, invocando static void Thread::dispatch(Thread* previous, Thread* next).
 **/
void Thread::exit(int status) {
    Debug::cout(Debug::Level::trace, "Thread::exit(" + std::to_string(status) + ")");
    // INSERT YOUR CODE HERE
    
    Thread *running = Thread::running();
    
    // só para garantir...
    if(running == nullptr)
        return;
    
    // deve colocar a thread que está executando no estado FINISHING
    running->_state = Thread::State::FINISHING;
    
    // atualiza a prioridade da thread
    // [caso seja possivel de uma thread voltar a rodar...]
    running->_updateNextGuess();
    
    // libera todas as threads esperando por esta thread
    running->unlockWaitingList();
    
    // chamar o escalonador para escolher outra thread
    Thread *next = System::scheduler()->choose();
    
    // e então o despachador para iniciar a execução da thread escolhida pelo escalonador
    Thread::dispatch(running, next);
}

/**
 * Threads são despachadas, ou seja, têm seus contextos trocados, quando se invoca a chamada 
 * static void Thread::dispatch(Thread* previous, Thread* next), que precisa ser implementada. A implementaçao desse  método 
 * deve inicialmente verificar se a próxima thread (next) é nula ou não. Se for, nada precisa ser feito (isso só ocorre quando 
 * a fila de prontos é vazia e não há thread para ser escalonada). Se não for, então o atributo _running deve ser atualizado e 
 * a thread a ser executada precisa ser colocada no estado RUNNING e retirada da fila de prontos. Então deve ser verificado se a thread anterior 
 * (previous) é diferente de nula e também se é diferente da próxma thread. Se não for, então basta restaurar o contexto da 
 * próxima thread, invocando static void CPU::restore_context(Thread* next). Se for, então é preciso verificar se a thread 
 * anterior estava no estado RUNNING e caso sim, então a thread anterior deve passar para o estado READY. Após esse teste 
 * deve-se fazer a troca de contexto entre as threads, invocando o método static void CPU::switch_context(Thread* previous, Thread* next).
 * **/
void Thread::dispatch(Thread* previous, Thread* next) {
     Debug::cout(Debug::Level::trace, "Thread::dispatch(" + std::to_string(reinterpret_cast<unsigned long> (previous)) + "," + std::to_string(reinterpret_cast<unsigned long> (next)) + ")");
    // INSERT YOUR CODE HERE
    
    // o atributo _running deve ser atualizado
    _running = next;
        
    // verificar se a próxima thread (next) é nula ou não. Se for, nada precisa ser feito
    if (next != nullptr) {
        // a thread a ser executada precisa ser colocada no estado RUNNING
        next->_state = Thread::State::RUNNING;
        // e retirada da fila de prontos
        System::scheduler()->remove(next);
        
        if(previous != next){
            // muda o tempo de entrada na CPU para o tempo atual do simulador
            //  mais o tempo necessario para troca de contexto
            next->_accountInfo._startTime = Simulator::getInstance()->getTnow() +
                Traits<CPU>::context_switch_overhead;
        }
        
        // deve ser verificado se a thread anterior (previous) é diferente de nula e também se é diferente da próxma thread
        if ((previous != nullptr) && (previous != next)) {
            //  verificar se a thread anterior estava no estado RUNNING
            if (previous->_state == Thread::State::RUNNING) {
                // a thread anterior deve passar para o estado READY
                previous->_state = Thread::State::READY;
            }
            // fazer a troca de contexto entre as threads
            CPU::switch_context(previous, next);
        } else {
            // Se não for, então basta restaurar o contexto da próxima thread
            CPU::restore_context(next);
        }
    }
}

/**
 * Threads criarão novas threads quando for invocada a chamada static Thread* Thread::thread_create(), que precisa ser
 * implementada. A implementação dessa chamada deve criar um objeto Thread, inicializar seus novos atributos, colocá-la na
 * lista de threads no sistema, inseri-la na fila de prontos inovando o método void Scheduler::insert(Thread* thread), 
 * e se o escolador for preemptivo, então deve chamar o escalonador para escolher outra thread, invocando 
 * Thread* Scheduler::choose() e então o despachador para iniciar a execução da thread escolhida pelo escalonador, 
 * invocando static void Thread::dispatch(Thread* previous, Thread* next).
 **/
Thread* Thread::thread_create(Process* parent) {
    Debug::cout(Debug::Level::trace, "Thread::create(" + std::to_string(reinterpret_cast<unsigned long> (parent)) + ")");
    // INSERT YOUR CODE HERE
    
    // Criar um objeto Thread
    Thread *thread = new Thread(parent);
    
    // Inicializar seus novos atributos
    
    // Colocá-la na lista de threads no sistema
    Thread::getThreadsList()->push_back(thread);
    
    // Inseri-la na fila de prontos
    System::scheduler()->insert(thread);
    
    // pega a thread rodando 
    Thread *running = Thread::running();
    
    if(running != nullptr){
        // atualiza a prioridade da thread
        running->_updateRemainTime();
        
        // caso a nova thread tenha prioridade menor ou igual a thread
        //  atualmente rodando, deve-se continuar a execução da thread
        //  atual
        if(running->getPriority() <= thread->getPriority()){
            // despachador para continuar a execução da thread running 
            //Thread::dispatch(running, running);
            return thread;
        }
        
        // colocar a thread que está executando no estado READY
        running->_state = Thread::State::READY;  
        
        // incluí-la na fila de threads prontas
        System::scheduler()->insert(running);
    }
    
    // chamar o escalonador para escolher outra thread
    Thread *next = System::scheduler()->choose();

    // despachador para iniciar a execução da thread escolhida pelo escalonador
    Thread::dispatch(running, next);
    
    return thread;
}