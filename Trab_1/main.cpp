#include <iostream>

#include "MemoryManager.h"

#define MEMORY_SIZE 2*1024*1024

int main()
{
    MemoryManager mManager(600);

    Process p1(1,0,0,1000);
    p1.addSegment(100);
    p1.addSegment(80);
    mManager.allocateMemoryForProcess(&p1);

    mManager.showAllocatedMemory();
    std::cout << "=============== add P1 ===============\n\n";

    Process p2(2,0,500,1000);
    p2.addSegment(250);
    p2.addSegment(80);
    mManager.allocateMemoryForProcess(&p2);

    mManager.showAllocatedMemory();
    std::cout << "=============== add P2 ===============\n\n";

    Process p3(3,0,500,1000);
    p3.addSegment(55);
    p3.addSegment(100);
    mManager.allocateMemoryForProcess(&p3);

    mManager.showAllocatedMemory();
    std::cout << "=============== add P3 ===============\n\n";

    mManager.deallocateMemoryOfProcess(&p1);
    mManager.showAllocatedMemory();
    std::cout << "=============== remove P1 ===============\n\n";

    mManager.deallocateMemoryOfProcess(&p2);
    mManager.showAllocatedMemory();
    std::cout << "=============== remove P2 ===============\n\n";

    mManager.deallocateMemoryOfProcess(&p3);
    mManager.showAllocatedMemory();
    std::cout << "=============== remove P3 ===============\n\n";

    return 0;
}
