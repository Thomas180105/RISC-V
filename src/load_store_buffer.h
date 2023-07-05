//
// Created by Lenovo on 2023/6/27.
//

#ifndef RISC_V_LOAD_STORE_BUFFER_H
#define RISC_V_LOAD_STORE_BUFFER_H
#include "queue.h"
#include "Instruction.h"
#include "ReorderBuffer.h"
class Bus;
struct loadStoreRow{
    InstructionType type;
    bool ready = false;
    bool locationDependence = false;
    bool valueDependence = false;
    Location locationQ1;
    Location valueQ2;
    Context location;
    Context value;
    signedContext offset;
    Location RoBIndex;
};
class loadStoreBuffer {
private:
    queue<loadStoreRow, 32> buffer_;
//    queue<loadStoreRow, 32> nextBuffer_;
    int clock_LSB = 0;

public:
    loadStoreRow& operator[](Location index);

    int innerTailIndex();

    bool Full() const;

    void clear();

//    void flush();

    void Add(const loadStoreRow &row, Bus &bus);

    void Run(Bus& bus);
private:
    void updateRoBTag(ReorderBuffer &reorderBuffer);
};


#endif //RISC_V_LOAD_STORE_BUFFER_H
