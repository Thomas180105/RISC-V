#include "reservationStation.h"

void ReservationStation::flush()
{
    for (int i = 0; i < maxSize; ++i) station_[i] = nextStation_[i];
    insertALU.flush();
}

//使用时保证不炸容量
int ReservationStation::Add(RSRow &row, Bus &bus)
{
    for (int i = 0 ; i < maxSize; ++i)
    {
        if (station_[i].empty)
        {
            nextStation_[i] = row;
            nextStation_[i].empty = false;
            return i;
        }
    }
    return -1;
}

//本质上就是沟通类中数据成员以及RoB的一个函数
void ReservationStation::run(ReorderBuffer &reorderBuffer)
{
    //use the alu to update the RoB
    if (insertALU.Finished())
    {
        reorderBuffer[station_[insertALU.tellIndex()].RoBIndex].value = insertALU.tellResult(); //NOLINT
        reorderBuffer[station_[insertALU.tellIndex()].RoBIndex].ready = true; //NOLINT
        nextStation_[insertALU.tellIndex()].empty = true;
    }

    //push the data in RS to the ALU
    if (!insertALU.Busy())
    {
        for (int i = 0; i < maxSize; ++i)
        {
            //如果可以找到合适的rowRS，则更新RS中的executing标志并且调用对应的ALU功能
            if (!station_[i].empty && !station_[i].busy && !station_[i].executing)
            {
                nextStation_[i].executing = true;
                InstructionType t = station_[i].type;
                int ALUType;
                if (t == InstructionType::ADD || t == InstructionType::SUB || t == InstructionType::ADDI) ALUType = 1;
                else if (t == InstructionType::SLL || t == InstructionType::SLLI|| t == InstructionType::SRL ||
                        t == InstructionType::SRLI || t == InstructionType::SRA || t == InstructionType::SRAI) ALUType = 2;
                else if (t == InstructionType::SLT || t == InstructionType::SLTI|| t == InstructionType::SLTU ||
                         t == InstructionType::SLTIU || t == InstructionType::BEQ || t == InstructionType::BNE ||
                         t == InstructionType::BLT || t == InstructionType::BGE || t == InstructionType::BLTU || t == InstructionType::BGEU) ALUType = 3;
                else ALUType = 4;// XOR XORI OR ORI AND ANDI
                //    else if (t == InstructionType::XOR || t == InstructionType::XORI|| t == InstructionType::OR ||
                //    t == InstructionType::ORI || t == InstructionType::AND || t == InstructionType::ANDI) ALUType = 4;

                switch (ALUType) { //NOLINT
                    case 1:
                        insertALU.Run_Add(station_[i].value1, station_[i].value2, i, t);
                        break;
                    case 2:
                        insertALU.Run_Shift(station_[i].value1, station_[i].value2, i, t);
                        break;
                    case 3:
                        insertALU.Run_Compare(station_[i].value1, station_[i].value2, i, t);
                        break;
                    case 4:
                        insertALU.Run_Logic(station_[i].value1, station_[i].value2, i, t);
                        break;
                }
                break;
            }
        }
    }

    //use RoB to update the busy Q1 Q2 in RS
    for (int i = 0; i < maxSize; ++i)
    {
        if (station_[i].empty || !station_[i].busy) continue;
        if (station_[i].Q1dependence && reorderBuffer[station_[i].Q1].ready)
        {
            nextStation_[i].Q1dependence = false;
            nextStation_[i].value1 = reorderBuffer[station_[i].Q1].value;
        }
        if (station_[i].Q2dependence && reorderBuffer[station_[i].Q2].ready)
        {
            nextStation_[i].Q2dependence = false;
            nextStation_[i].value2 = reorderBuffer[station_[i].Q2].value;
        }
        if (!nextStation_[i].Q1dependence && !nextStation_[i].Q2dependence) nextStation_[i].busy = false;
    }
}

void ReservationStation::clear()
{
    for (auto & i : nextStation_) i.empty = true;
    insertALU.clear();
}