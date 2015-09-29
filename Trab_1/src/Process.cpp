#include "Process.h"

Process::Process(unsigned int id, unsigned int parentId, double tArrival, double tEnd):
    _id(id), _parentId(parentId), _tArrival(tArrival), _tEnd(tEnd) {
}

Process::Process(const Process& orig){}

Process::~Process(){
    for(auto i = _segments.begin(); i != _segments.end(); i++)
        delete (*i);
}
