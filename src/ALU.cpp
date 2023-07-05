#include "ALU.h"

void ALU::clear() {busy = false;}

bool ALU::Busy() const {return busy;}

bool ALU::Finished() const {return finished;}

Context ALU::tellResult() const {return result;}

Context ALU::tellIndex() const {return index;}

void ALU::flush()
{
    if (busy) {busy = false, finished = true;}
    else finished = false;
    result = nextResult;
    index = nextIndex;
}

void ALU::Run_Add(Context x1, Context x2, Location place, InstructionType type)
{
    nextIndex = place;
    busy = true;
    if (type == InstructionType::ADD || type == InstructionType::ADDI) nextResult = x1 + x2;
    else if (type == InstructionType::SUB) nextResult = x1 - x2;
}

void ALU::Run_Shift(Context x1, Context x2, Location place, InstructionType type)
{
    nextIndex = place;
    busy = true;
    if (type == InstructionType::SLL || type == InstructionType::SLLI) nextResult = x1 << x2;
    else if (type == InstructionType::SRL || type == InstructionType::SRLI) nextResult = x1 >> x2;
    else nextResult = static_cast<signedContext>(x1) >> x2;
}

void ALU::Run_Compare(Context x1, Context x2, Location place, InstructionType type)
{
    //this is the compare
    nextIndex = place;
    busy = true;
    switch (type)
    {
        case InstructionType::BEQ:
            nextResult = static_cast<Context>(x1 == x2);
            break;
        case InstructionType::BNE:
            nextResult = static_cast<Context>(x1 != x2);
            break;
        case InstructionType::BGE:
            nextResult = static_cast<Context>(static_cast<signedContext>(x1) >= static_cast<signedContext>(x2));
            break;
        case InstructionType::BGEU:
            nextResult = static_cast<Context>(x1 >= x2);
            break;
        case InstructionType::SLT:
        case InstructionType::SLTI:
        case InstructionType::BLT:
            nextResult = static_cast<Context>(static_cast<signedContext>(x1) < static_cast<signedContext>(x2));
            break;
        case InstructionType::SLTU:
        case InstructionType::SLTIU:
        case InstructionType::BLTU:
            nextResult = static_cast<Context>(x1 < x2);
            break;
    }
}

void ALU::Run_Logic(Context x1, Context x2, Location place, InstructionType type)
{
    nextIndex = place;
    busy = true;
    if (type == InstructionType::ADD || type == InstructionType::ANDI) nextResult = x1 & x2;
    else if (type == InstructionType::OR || type == InstructionType::ORI) nextResult = x1 | x2;
    else if (type == InstructionType::XOR || type == InstructionType::XORI) nextResult = x1 ^ x2;
}