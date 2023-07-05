#ifndef RISC_V_INSTRUCTION_H
#define RISC_V_INSTRUCTION_H
#include "type.h"
#include "registerFile.h"
#include "predictor.h"
class Bus;
enum class InstructionType{
    LUI, AUIPC, JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU, LB, LH, LW, LBU, LHU, SB, SH, SW, ADDI,
    SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
};

class InstructionInfo{
public:
    InstructionType typeId;
    Location sourceRegister1 = 0;
    Location sourceRegister2 = 0;
    Location destinationRegister = 0;
    Context offset = 0;
    string typeStrName[40] = {"lui", "auipc", "jal", "jalr", "beq", "bne", "blt",
                              "bge", "bltu", "bgeu", "lb", "lh", "lw", "lbu",
                              "lhu", "sb", "sh", "sw", "addi", "slti", "sltiu",
                              "xori", "ori", "andi", "slli", "srli", "srai", "add",
                              "sub", "sll", "slt", "sltu", "xor", "srl", "sra",
                              "or","and"};
    void print() const
    {
        std::cout<<typeStrName[int(typeId)]<<" "<<sourceRegister1<<" "<<sourceRegister2<<" "<<destinationRegister<<" "<<static_cast<signedContext>(offset)<<'\n';
    }
};

InstructionInfo getInstructionInfo(const Context &input);

class InstructionManager{
private:
    Register PC_;
    Predictor predictor_;
    //jalr应该不能预测，这里如果遇到且寄存器有依赖，就直接卡进程
    bool jalrFlag = false;
    Context offset_;
    Location dependency_;
    Location jalrRow;
public:
    void launch(Bus &bus);
    void SetPC(Context pc);
    void clear();
    Register &getRegister() {return PC_;}
    Predictor& GetPredictor();
};

#endif //RISC_V_INSTRUCTION_H


