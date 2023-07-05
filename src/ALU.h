#ifndef RISC_V_ALU_H
#define RISC_V_ALU_H
#include "type.h"
#include "Instruction.h"
//#define InstructionType int
class ALU {
protected:
    bool busy = false;
    bool finished = false;
    Context result, index, nextResult, nextIndex;//存其在RS中的下标
public:
    void clear();
    void flush();
    bool Busy() const;
    bool Finished() const;
    Context tellResult() const;
    Context tellIndex() const;
    void Run_Add(Context x1, Context x2, Location place, InstructionType type);
    void Run_Shift(Context x1, Context x2, Location place, InstructionType type);
    void Run_Compare(Context x1, Context x2, Location place, InstructionType type);
    void Run_Logic(Context x1, Context x2, Location place, InstructionType type);
};

#endif //RISC_V_ALU_H
