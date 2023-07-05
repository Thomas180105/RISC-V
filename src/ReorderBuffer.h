#ifndef RISC_V_REORDERBUFFER_H
#define RISC_V_REORDERBUFFER_H
#include "type.h"
#include "queue.h"
#include "Instruction.h"
class Bus;
enum RoBType{
    registerWrite, memoryWrite, branch, end
};

struct RoBRow{
    bool ready = false;
    RoBType type;
    Location index;//含义根据type不同而不同，主要可以参见instruction.cpp
    Context value;
    Location address;//for branch, 存放未被选中的PC_跳转值
    bool branchAdvise;//for branch
    Location tellAfterPC = -1;
    InstructionInfo info;
};

class Bus;
class ReorderBuffer {
private:
    queue<RoBRow, 32> buffer_;
    queue<RoBRow, 32> nextBuffer_;
public:
    int Add(const RoBRow &row, Bus &bus);
    void tyrCommit(Bus &bus);
    RoBRow &operator[](const int &index);
    RoBRow &getNewRow(const int &index);
    const RoBRow&tellOldRow(const int &index);
    bool full();
    void flush();
    void clear();
    void printDebugInfo(RoBRow &row, Bus &bus) const;
};


#endif //RISC_V_REORDERBUFFER_H
