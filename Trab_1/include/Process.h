#ifndef PROCESS_H
#define    PROCESS_H
#include <list>

class MemorySegment {
public:
    MemorySegment(unsigned int size) {
        this->_size = size;
    }
    MemorySegment(const MemorySegment& orig) {  }
    virtual ~MemorySegment() {  }
    unsigned int getSize() const {
        return _size;
    }
private:
    unsigned int _size;
};

class Process {
public:
    Process(unsigned int id, unsigned int parentId, double tArrival, double tEnd);
    Process(const Process& orig);
    virtual ~Process();
    double getTArrival() const{
        return this->_tArrival;
    }
    void setEndMemory(unsigned int _endMemory){
        this->_endMemory = _endMemory;
    }
    unsigned int getEndMemory() const{
        return _endMemory;
    }
    void setBeginMemory(unsigned int _beginMemory){
        this->_beginMemory = _beginMemory;
    }
    unsigned int getBeginMemory() const{
        return this->_beginMemory;
    }
    std::list<MemorySegment*>* getSegments(){
        return &_segments;
    }
    unsigned int getParentId() const{
        return this->_parentId;
    }
    unsigned int getId() const{
        return _id;
    }

    void addSegment(unsigned int size){
        _segments.push_back(new MemorySegment(size));
    }

private:
    unsigned int _endMemory=0;
    unsigned int _beginMemory=0;

    unsigned int _id=0;
    unsigned int _parentId=0;

    double _tArrival=0;
    double _tEnd=0;

    std::list<MemorySegment*> _segments;
};

#endif    /* PROCESS_H */
