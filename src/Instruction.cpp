#include "Instruction.h"
#include "load_store_buffer.h"
#include "ReorderBuffer.h"
#include "reservationStation.h"
#include "type.h"
#include "bus.h"
//#define debug
//#define outputError
//#define outputPC
//#define find20000
//imm[31:12]<31:12>
Context getImm_typeU(const Context &input) {return input & 0xfffff000;}
//imm[20|10:1|11|19:12]<31:12> signedExtend version only
Context getImm_typeJ(const Context &input)
{
    Context tmp = ((input & 0x80000000) >> 11) | ((input & 0x7fe00000) >> 20) | ((input & 0x00100000) >> 9) | ((input & 0x000ff000));
    return ((tmp & 0x100000) ? (tmp | 0xfff00000) : tmp);
}
//imm[11:0]<31:20> signedExtend version and non-Extend version
Context getImm_typeI(const Context &input) {return (input & 0xfff00000) >> 20;}
Context getSignedImm_typeI(const Context &input)
{
    Context tmp = (input & 0xfff00000) >> 20;
    return (tmp & 0x800) ? (tmp | 0xfffff000) : tmp;
}
//imm[12|10:5]<31:25> imm[4:1|11]<11:7> signedExtend version only
Context getImm_typeB(const Context &input)
{
    Context tmp = ((input & 0x80000000) >> 19) | ((input & 0x7e000000) >> 20) | ((input & 0x00000f00) >> 7) | ((input & 0x00000080) << 4);
    return ((tmp & 0x1000) ? (tmp | 0xfffff000) : tmp);
}
//imm[11:5]<31:25> imm[4:0]<11:7> signedExtend version only
Context getImm_typeS(const Context &input)
{
    Context tmp = ((input & 0xfe000000) >> 20) | ((input & 0x00000f80) >> 7);
    return (tmp & 0x800) ? (tmp | 0xfffff000) : tmp;
}
//rs1<19:15>
Location getRs1(const Context &input) {return (input & 0x000f8000) >> 15;}
//rs2<24:20>
Location getRs2(const Context &input) {return (input & 0x01f00000) >> 20;}
//rd<11:7>
Location getRd(const Context &input) {return (input & 0x00000f80) >> 7;}
//funct3<14:12>
Context getFunct3(const Context &input) {return (input & 0x00007000) >> 12;}
//funct7<6:0>
Context getFunct7(const Context &input) {return input & 0x0000007f;}
//top7<31::25>
Context getTop7(const Context &input) {return (input >> 25) & 0b1111111;}
//shamt<24:20>
Context getShamt(const Context &input) {return (input & 0x01f00000) >> 20;}

//Context SignExtend(const Context &imm) { return (imm & 0x800) ? (imm | 0xFFFFF000) : imm; }
#include <bitset>
InstructionInfo getInstructionInfo(const Context &input)
{
#ifdef debug
    std::cout<<"at the beginning in launch(), the info is "<<bitset<32>(input)<<'\n';
#endif
    Context func7 = getFunct7(input);
    Context func3 = getFunct3(input);
    InstructionInfo res;

    switch(func7)
    {
        case 0b0110111:
            res.typeId = InstructionType::LUI;
            res.offset = getImm_typeU(input);
            res.destinationRegister = getRd(input);
            return res;
        case 0b0010111:
            res.typeId = InstructionType::AUIPC;
            res.offset = getImm_typeU(input);
            res.destinationRegister = getRd(input);
            return res;
        case 0b1101111:
            res.typeId = InstructionType::JAL;
            res.offset = getImm_typeJ(input);
            res.destinationRegister = getRd(input);
            return res;
        case 0b1100111:
            res.typeId = InstructionType::JALR;
            res.offset = getSignedImm_typeI(input);
            res.destinationRegister = getRd(input);
            res.sourceRegister1 = getRs1(input);
            return res;
        case 0b1100011://BEQ BNE BLT BGE BLTU BGEU
            switch (func3)
            {
                case 0b000:
                    res.typeId = InstructionType::BEQ;
                    res.offset = getImm_typeB(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b001:
                    res.typeId = InstructionType::BNE;
                    res.offset = getImm_typeB(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b100:
                    res.typeId = InstructionType::BLT;
                    res.offset = getImm_typeB(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b101:
                    res.typeId = InstructionType::BGE;
                    res.offset = getImm_typeB(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b110:
                    res.typeId = InstructionType::BLTU;
                    res.offset = getImm_typeB(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b111:
                    res.typeId = InstructionType::BGEU;
                    res.offset = getImm_typeB(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                default:
                    std::cout<<"an error occurs in decoder.#1\n";
                    return res;
            }
        case 0b0000011://LB LH LW LBU LHU
            switch (func3)
            {
                case 0b000:
                    res.typeId = InstructionType::LB;
                    res.offset = getSignedImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b001:
                    res.typeId = InstructionType::LH;
                    res.offset = getSignedImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b010:
                    res.typeId = InstructionType::LW;
                    res.offset = getSignedImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b100:
                    res.typeId = InstructionType::LBU;
                    res.offset = getSignedImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b101:
                    res.typeId = InstructionType::LHU;
                    res.offset = getSignedImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                default:
                    std::cout<<"an error occurs in decoder.#2\n";
                    return res;
            }
        case 0b0100011://SB SH SW
            switch (func3)
            {
                case 0b000:
                    res.typeId = InstructionType::SB;
                    res.offset = getImm_typeS(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b001:
                    res.typeId = InstructionType::SH;
                    res.offset = getImm_typeS(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b010:
                    res.typeId = InstructionType::SW;
                    res.offset = getImm_typeS(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                default:
                    std::cout<<"an error occurs in decoder.#3\n";
                    return res;
            }
        case 0b0010011://ADDI ALTI SLTIU XORI ORI ANDI SLLI SRLI SRAI
            switch (func3)
            {
                case 0b000:
                    res.typeId = InstructionType::ADDI;
                    res.offset = getSignedImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b010:
                    res.typeId = InstructionType::SLTI;
                    res.offset = getSignedImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b011:
                    res.typeId = InstructionType::SLTIU;
                    res.offset = getImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b100:
                    res.typeId = InstructionType::XORI;
                    res.offset = getSignedImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b110:
                    res.typeId = InstructionType::ORI;
                    res.offset = getSignedImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b111:
                    res.typeId = InstructionType::ANDI;
                    res.offset = getSignedImm_typeI(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b001:
                    res.typeId = InstructionType::SLLI;
                    res.offset = getShamt(input);
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    return res;
                case 0b101:
                {
                    Context top7 = getTop7(input);
                    if (top7 == 0b0000000)
                    {
                        res.typeId = InstructionType::SRLI;
                        res.offset = getShamt(input);
                        res.destinationRegister = getRd(input);
                        res.sourceRegister1 = getRs1(input);
                        return res;
                    }
                    else if (top7 == 0b0100000)
                    {
                        res.typeId = InstructionType::SRAI;
                        res.offset = getShamt(input);
                        res.destinationRegister = getRd(input);
                        res.sourceRegister1 = getRs1(input);
                        return res;
                    }
                    else
                    {
                        std::cout<<"an error occurs in decoder.#4\n";
                        return res;
                    }
                }
                default:
                    std::cout<<"an error occurs in decoder.#5\n";
                    return res;
            }
        case 0b0110011:
            switch (func3)
            {
                case 0b000:
                {
                    Context top7 = getTop7(input);
                    if (top7 == 0b0000000)
                    {
                        res.typeId = InstructionType::ADD;
                        res.destinationRegister = getRd(input);
                        res.sourceRegister1 = getRs1(input);
                        res.sourceRegister2 = getRs2(input);
                        return res;
                    }
                    else if (top7 == 0b0100000)
                    {
                        res.typeId = InstructionType::SUB;
                        res.destinationRegister = getRd(input);
                        res.sourceRegister1 = getRs1(input);
                        res.sourceRegister2 = getRs2(input);
                        return res;
                    }
                    else
                    {
                        std::cout<<"an error occurs in decoder.#6\n";
                        return res;
                    }
                }
                case 0b001:
                    res.typeId = InstructionType::SLL;
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b010:
                    res.typeId = InstructionType::SLT;
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b011:
                    res.typeId = InstructionType::SLTU;
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b100:
                    res.typeId = InstructionType::XOR;
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b101:
                {
                    Context top7 = getTop7(input);
                    if (top7 == 0b0000000)
                    {
                        res.typeId = InstructionType::SRL;
                        res.destinationRegister = getRd(input);
                        res.sourceRegister1 = getRs1(input);
                        res.sourceRegister2 = getRs2(input);
                        return res;
                    }
                    else if (top7 == 0b0100000)
                    {
                        res.typeId = InstructionType::SRA;
                        res.destinationRegister = getRd(input);
                        res.sourceRegister1 = getRs1(input);
                        res.sourceRegister2 = getRs2(input);
                        return res;
                    }
                    else
                    {
                        std::cout<<"an error occurs in decoder.#7\n";
                        return res;
                    }
                }
                case 0b110:
                    res.typeId = InstructionType::OR;
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                case 0b111:
                    res.typeId = InstructionType::SRA;
                    res.destinationRegister = getRd(input);
                    res.sourceRegister1 = getRs1(input);
                    res.sourceRegister2 = getRs2(input);
                    return res;
                default:
                    std::cout<<"an error occurs in decoder.#8\n";
                    return res;
            }
        default:
        {
#ifdef outputError
            std::cout<<"an error occurs in decoder.#9";
            std::cout<<" And the func7 is "<<bitset<7>(func7)<<endl;
#endif
            return res;
        }
    }
}

void InstructionManager::launch(Bus &bus)
{
    //先判断是否被jalr卡住，如果是，尝试检查是否堵塞已经解决
    //若没有被jalr卡住且RoB等地方还有额外的位置，则可以直接读指令，然后decode，然后根据类型来维护RoB以及其它结构
    if (jalrFlag)
    {
        if (bus.getReorderBuffer()[dependency_].ready)
        {
            jalrFlag = false;
            PC_.getValue() = static_cast<signedContext>(offset_) + bus.getReorderBuffer()[dependency_].value;
//            bus.getReorderBuffer()[jalrRow].tellAfterPC = PC_.getValue();
            bus.getReorderBuffer()[jalrRow].ready = true;
#ifdef find20000
            if (PC_.getValue() == 0x20000)//0x20000 = 131072
            {
                std::cout<<"this is a neg circle #15\n";
            }
#endif
        }
        return;
    }

    //RS的承载量不会高于RoB，所以判断了RoB就相当于保证了RS
    if (bus.getReorderBuffer().full() || bus.getLoadStoreBuffer().Full()) return;
#ifdef find20000
    if (PC_.getValue() == 0x20000)//0x20000 = 131072
    {
        std::cout<<"this is a neg circle \n";
    }
#endif
    Context info = bus.getSourceManager().readInstruction(PC_.getValue());
#ifdef outputPC
    std::cout<<"in the launch(), the PC is "<<std::hex<<PC_.getValue()<<"\n";
#endif
#ifdef debug
    std::cout<<"at the beginning in launch(), the info is "<<std::hex<<info<<'\n';
#endif

    if (info == 0x0ff00513)
    {
        RoBRow row;
        row.type = RoBType::end;
        row.ready = true;
        bus.getReorderBuffer().Add(row, bus);
        return;
    }

    InstructionInfo curInstruction = getInstructionInfo(info);

    if (PC_.getValue() == 4236)
    {
        std::cout<<"";
    }

    switch (curInstruction.typeId)
    {
        case InstructionType::LUI://直接ready，不用放RS中
        {
            RoBRow row;
//            row.info = curInstruction;
            row.type = RoBType::registerWrite;
            row.ready = true;
            row.value = curInstruction.offset;
            row.index = curInstruction.destinationRegister;
            PC_.getValue() += 4;
//            row.tellAfterPC = PC_.getValue();
            bus.getReorderBuffer().Add(row, bus);
#ifdef find20000
            if (PC_.getValue() == 0x20000)//0x20000 = 131072
            {
                std::cout<<"this is a neg circle #16\n";
            }
#endif
            break;
        }
        case InstructionType::AUIPC:
        {
            RoBRow row;
//            row.info = curInstruction;
            row.ready = true;
            row.type = RoBType::registerWrite;
            row.value = curInstruction.offset + PC_.getValue();
            row.index = curInstruction.destinationRegister;
            PC_.getValue() += 4;
//            row.tellAfterPC = PC_.getValue();
            bus.getReorderBuffer().Add(row, bus);
#ifdef find20000
            if (PC_.getValue() == 0x20000)//0x20000 = 131072
            {
                std::cout<<"this is a neg circle #17\n";
            }
#endif
            break;
        }
        case InstructionType::JAL:
        {
            RoBRow row;
//            row.info = curInstruction;
            row.ready = true;
            row.type = RoBType::registerWrite;
            row.value = PC_.getValue() + 4;
            row.index = curInstruction.destinationRegister;
            PC_.getValue() += static_cast<signedContext>(curInstruction.offset);
//            row.tellAfterPC = PC_.getValue();
            bus.getReorderBuffer().Add(row, bus);
#ifdef find20000
            if (PC_.getValue() == 0x20000)//0x20000 = 131072
            {
                std::cout<<"this is a neg circle #11\n";
            }
#endif
            break;
        }
        case InstructionType::JALR:
        {
            RoBRow row;
//            row.info = curInstruction;
            row.type = RoBType::registerWrite;
            row.index = curInstruction.destinationRegister;
            row.value = PC_.getValue() + 4;
            if (!bus.getRegisterFile()[curInstruction.sourceRegister1].tellOccupy())
            {
                PC_.getValue() = (bus.getRegisterFile()[curInstruction.sourceRegister1].getValue() + static_cast<signedContext>(curInstruction.offset)) & ~1;
//                row.tellAfterPC = PC_.getValue();
                row.ready = true;
                bus.getReorderBuffer().Add(row, bus);
#ifdef find20000
                if (PC_.getValue() == 0x20000)//0x20000 = 131072
                {
                    std::cout<<"this is a neg circle #12\n";
                }
#endif
            }
            else
            {
                int tmpProblem = bus.getRegisterFile()[curInstruction.sourceRegister1].tellDependency();
                if (bus.getReorderBuffer()[tmpProblem].ready)
                {
                    PC_.getValue() = (bus.getReorderBuffer()[tmpProblem].value + static_cast<signedContext>(curInstruction.offset)) & ~1;
//                    row.tellAfterPC = PC_.getValue();
                    row.ready = true;
                    bus.getReorderBuffer().Add(row, bus);
#ifdef find20000
                    if (PC_.getValue() == 0x20000)//0x20000 = 131072
                    {
                        std::cout<<"this is a neg circle #18\n";
                        std::cout<<"the first adder is "<<bus.getReorderBuffer()[tmpProblem].value;
                        std::cout<<"the second adder is "<<static_cast<signedContext>(curInstruction.offset)<<'\n';
                    }
#endif
                }
                else
                {
                    row.ready = false;
                    jalrFlag = true;
                    offset_ = curInstruction.offset;
                    dependency_ = tmpProblem;
                    jalrRow = bus.getReorderBuffer().Add(row, bus);
                }
            }
            break;
        }
            //BEQ BNE BLT BGE BLTU BGEU
        case InstructionType::BEQ:
        case InstructionType::BNE:
        case InstructionType::BLT:
        case InstructionType::BGE:
        case InstructionType::BLTU:
        case InstructionType::BGEU:
        {
            //试图取出
            RoBRow rowRoB;
//            rowRoB.info = curInstruction;
            rowRoB.ready = false;
            rowRoB.type = RoBType::branch;
            RSRow rowRS;
            rowRS.type = curInstruction.typeId;
            if (!bus.getRegisterFile()[curInstruction.sourceRegister1].tellOccupy())
            {
                rowRS.value1 = bus.getRegisterFile()[curInstruction.sourceRegister1].getValue();
            }
            else
            {
                rowRS.Q1 = bus.getRegisterFile()[curInstruction.sourceRegister1].tellDependency();
                if (bus.getReorderBuffer()[rowRS.Q1].ready)  rowRS.value1 = bus.getReorderBuffer()[rowRS.Q1].value;
                else {rowRS.busy = true, rowRS.Q1dependence = true;}
            }
            if (!bus.getRegisterFile()[curInstruction.sourceRegister2].tellOccupy())
            {
                rowRS.value2 = bus.getRegisterFile()[curInstruction.sourceRegister2].getValue();
            }
            else
            {
                rowRS.Q2 = bus.getRegisterFile()[curInstruction.sourceRegister2].tellDependency();
                if (bus.getReorderBuffer()[rowRS.Q2].ready) rowRS.value2 = bus.getReorderBuffer()[rowRS.Q2].value;
                else {rowRS.busy = true, rowRS.Q2dependence = true;}
            }
            rowRoB.address = PC_.getValue();
            rowRoB.branchAdvise = predictor_.Predict(PC_.getValue());
            if (rowRoB.branchAdvise)//决定跳转
            {
                rowRoB.index = PC_.getValue() + 4;
                PC_.getValue() += static_cast<signedContext>(curInstruction.offset);
//                rowRoB.tellAfterPC = PC_.getValue();
#ifdef find20000
                if (PC_.getValue() == 0x20000)//0x20000 = 131072
                {
                    std::cout<<"this is a neg circle #13\n";
                }
#endif
            }
            else//不跳转
            {
                rowRoB.index = PC_.getValue() + static_cast<signedContext>(curInstruction.offset);
                PC_.getValue() += 4;
//                rowRoB.tellAfterPC = PC_.getValue();
#ifdef find20000
                if (PC_.getValue() == 0x20000)//0x20000 = 131072
                {
                    std::cout<<"this is a neg circle #14\n";
                }
#endif
            }
            rowRS.RoBIndex = bus.getReorderBuffer().Add(rowRoB, bus);
            bus.getReservationStation().Add(rowRS, bus);
            break;
        }
            //LB LH LW LBU LHU
        case InstructionType::LB:
        case InstructionType::LH:
        case InstructionType::LW:
        case InstructionType::LBU:
        case InstructionType::LHU:
        {
            RoBRow rowRoB;
//            rowRoB.info = curInstruction;
            rowRoB.type = RoBType::registerWrite;
            rowRoB.ready = false;
            rowRoB.index = curInstruction.destinationRegister;
            //需要使用一个LSB的row
            loadStoreRow rowLS;
            rowLS.type = curInstruction.typeId;
            rowLS.offset = static_cast<signedContext>(curInstruction.offset);
            rowLS.ready = true;
            if (!bus.getRegisterFile()[curInstruction.sourceRegister1].tellOccupy())
            {
                rowLS.location = bus.getRegisterFile()[curInstruction.sourceRegister1].getValue();
            }
            else
            {
                rowLS.locationQ1 = bus.getRegisterFile()[curInstruction.sourceRegister1].tellDependency();
                if (bus.getReorderBuffer()[rowLS.locationQ1].ready) rowLS.location = bus.getReorderBuffer()[rowLS.locationQ1].value;
                else {rowLS.locationDependence = true; rowLS.ready = false;}
            }
            PC_.getValue() += 4;
//            rowRoB.tellAfterPC = PC_.getValue();
            rowLS.RoBIndex = bus.getReorderBuffer().Add(rowRoB, bus);
            bus.getLoadStoreBuffer().Add(rowLS, bus);
#ifdef find20000
            if (PC_.getValue() == 0x20000)//0x20000 = 131072
            {
                std::cout<<"this is a neg circle #19\n";
            }
#endif
            break;
        }
            //SB SH SW
        case InstructionType::SB:
        case InstructionType::SH:
        case InstructionType::SW:
        {
            RoBRow rowRoB;
//            rowRoB.info = curInstruction;
            rowRoB.ready = false;
            rowRoB.type = RoBType::memoryWrite;
            loadStoreRow rowLS;
            rowLS.type = curInstruction.typeId;
            rowLS.offset = static_cast<signedContext>(curInstruction.offset);
            if (!bus.getRegisterFile()[curInstruction.sourceRegister1].tellOccupy())
            {
                rowLS.location = bus.getRegisterFile()[curInstruction.sourceRegister1].getValue();
            }
            else
            {
                rowLS.locationQ1 = bus.getRegisterFile()[curInstruction.sourceRegister1].tellDependency();
                if (bus.getReorderBuffer()[rowLS.locationQ1].ready) rowLS.location = bus.getReorderBuffer()[rowLS.locationQ1].value;
                else {rowLS.locationDependence = true; rowLS.ready = false;}
            }
            if (!bus.getRegisterFile()[curInstruction.sourceRegister2].tellOccupy())
            {
                rowLS.value = bus.getRegisterFile()[curInstruction.sourceRegister2].getValue();
            }
            else
            {
                rowLS.valueQ2 = bus.getRegisterFile()[curInstruction.sourceRegister2].tellDependency();
                if (bus.getReorderBuffer()[rowLS.valueQ2].ready) rowLS.value = bus.getReorderBuffer()[rowLS.valueQ2].value;
                else {rowLS.valueDependence = true; rowLS.ready = false;}
            }
            PC_.getValue() += 4;
//            rowRoB.tellAfterPC = PC_.getValue();
            rowRoB.index = bus.getLoadStoreBuffer().innerTailIndex();
            rowLS.RoBIndex = bus.getReorderBuffer().Add(rowRoB, bus);
            bus.getLoadStoreBuffer().Add(rowLS, bus);
#ifdef find20000
            if (PC_.getValue() == 0x20000)//0x20000 = 131072
            {
                std::cout<<"this is a neg circle #20\n";
            }
#endif
            break;
        }
        case InstructionType::ADDI:
        {
            RoBRow rowRoB;
//            rowRoB.info = curInstruction;
            rowRoB.ready = false;
            rowRoB.type = RoBType::registerWrite;
            rowRoB.index = curInstruction.destinationRegister;
            RSRow rowRS;
            rowRS.type = curInstruction.typeId;
            rowRS.value2 = curInstruction.offset;//不用强转为signedContext，因为还没有进行实际计算
            if (!bus.getRegisterFile()[curInstruction.sourceRegister1].tellOccupy())
            {
                rowRS.value1 = bus.getRegisterFile()[curInstruction.sourceRegister1].getValue();
            }
            else
            {
                rowRS.Q1 = bus.getRegisterFile()[curInstruction.sourceRegister1].tellDependency();
                if (bus.getReorderBuffer()[rowRS.Q1].ready) rowRS.value1 = bus.getReorderBuffer()[rowRS.Q1].value;
                else {rowRS.Q1dependence = true; rowRS.busy = true;}
            }
            PC_.getValue() += 4;
//            rowRoB.tellAfterPC = PC_.getValue();
            rowRS.RoBIndex = bus.getReorderBuffer().Add(rowRoB, bus);
            bus.getReservationStation().Add(rowRS, bus);
#ifdef find20000
            if (PC_.getValue() == 0x20000)//0x20000 = 131072
            {
                std::cout<<"this is a neg circle #21\n";
            }
#endif
            break;
        }
        case InstructionType::SLTI:
        case InstructionType::SLTIU:
        case InstructionType::XORI:
        case InstructionType::ORI:
        case InstructionType::ANDI:
        case InstructionType::SLLI:
        case InstructionType::SRLI:
        case InstructionType::SRAI:
        {
            RoBRow rowRoB;
//            rowRoB.info = curInstruction;
            rowRoB.ready = false;
            rowRoB.type = RoBType::registerWrite;
            rowRoB.index = curInstruction.destinationRegister;
            RSRow rowRS;
            rowRS.type = curInstruction.typeId;
            rowRS.value2 = curInstruction.offset;
            if (!bus.getRegisterFile()[curInstruction.sourceRegister1].tellOccupy())
            {
                rowRS.value1 = bus.getRegisterFile()[curInstruction.sourceRegister1].getValue();
            }
            else
            {
                rowRS.Q1 = bus.getRegisterFile()[curInstruction.sourceRegister1].tellDependency();
                if (bus.getReorderBuffer()[rowRS.Q1].ready) rowRS.value1 = bus.getReorderBuffer()[rowRS.Q1].value;
                else {rowRS.Q1dependence = true; rowRS.busy = true;}
            }
            PC_.getValue() += 4;
//            rowRoB.tellAfterPC = PC_.getValue();
            rowRS.RoBIndex = bus.getReorderBuffer().Add(rowRoB, bus);
            bus.getReservationStation().Add(rowRS, bus);
#ifdef find20000
            if (PC_.getValue() == 0x20000)//0x20000 = 131072
            {
                std::cout<<"this is a neg circle #22\n";
            }
#endif
            break;
        }
        case InstructionType::ADD:
        case InstructionType::SUB:
        case InstructionType::SLL:
        case InstructionType::SLT:
        case InstructionType::SLTU:
        case InstructionType::XOR:
        case InstructionType::SRL:
        case InstructionType::SRA:
        case InstructionType::OR:
        case InstructionType::AND:
        {
            RoBRow rowRoB;
//            rowRoB.info = curInstruction;
            rowRoB.ready = false;
            rowRoB.type = RoBType::registerWrite;
            rowRoB.index = curInstruction.destinationRegister;
            RSRow rowRS;
            rowRS.type = curInstruction.typeId;
            if (!bus.getRegisterFile()[curInstruction.sourceRegister1].tellOccupy())
            {
                rowRS.value1 = bus.getRegisterFile()[curInstruction.sourceRegister1].getValue();
            }
            else
            {
                rowRS.Q1 = bus.getRegisterFile()[curInstruction.sourceRegister1].tellDependency();
                if (bus.getReorderBuffer()[rowRS.Q1].ready) rowRS.value1 = bus.getReorderBuffer()[rowRS.Q1].value;
                else {rowRS.Q1dependence = true; rowRS.busy = true;}
            }
            if (!bus.getRegisterFile()[curInstruction.sourceRegister2].tellOccupy())
            {
                rowRS.value2 = bus.getRegisterFile()[curInstruction.sourceRegister2].getValue();
            }
            else
            {
                rowRS.Q2 = bus.getRegisterFile()[curInstruction.sourceRegister2].tellDependency();
                if (bus.getReorderBuffer()[rowRS.Q2].ready) rowRS.value2 = bus.getReorderBuffer()[rowRS.Q2].value;
                else {rowRS.Q2dependence = true; rowRS.busy = true;}
            }
            PC_.getValue() += 4;
//            rowRoB.tellAfterPC = PC_.getValue();
            rowRS.RoBIndex = bus.getReorderBuffer().Add(rowRoB, bus);
            bus.getReservationStation().Add(rowRS, bus);
#ifdef find20000
            if (PC_.getValue() == 0x20000)//0x20000 = 131072
            {
                std::cout<<"this is a neg circle #23\n";
            }
#endif
            break;
        }
        default:
        {
#ifdef outputError
            std::cout<<"an error occurs in launch().(please check Instruction.cpp)#10\n";
#endif
        }
    }
}

void InstructionManager::SetPC(Context pc) {PC_.getValue() = pc;}

void InstructionManager::clear() {jalrFlag = false;}

Predictor &InstructionManager::GetPredictor() {return predictor_;}
