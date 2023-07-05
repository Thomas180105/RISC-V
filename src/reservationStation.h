#ifndef RISC_V_RESERVATIONSTATION_H
#define RISC_V_RESERVATIONSTATION_H
#include "Instruction.h"
#include "ALU.h"
#include "queue.h"
#include "ReorderBuffer.h"

class Bus;
//enum class InstructionType;
struct RSRow {
    bool busy = false;
    InstructionType type;
    Context value1;
    Context value2;
    bool Q1dependence = false;
    bool Q2dependence = false;
    Context Q1;
    Context Q2;
    Location RoBIndex;
    bool empty = true;
    bool executing = false;
};

//不可以使用队列的结构
class ReservationStation{
    const static int maxSize = 32;
private:
    RSRow station_[maxSize];
    RSRow nextStation_[maxSize];
    ALU insertALU;
public:
    void flush();
    int Add(RSRow &row, Bus &bus);
    void run(ReorderBuffer &reorderBuffer);
    void clear();
};

#endif //RISC_V_RESERVATIONSTATION_H
