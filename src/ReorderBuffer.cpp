#include "ReorderBuffer.h"
#include "bus.h"
#include "source.h"
//如果是写寄存器，则更新registerFile的工作在此时完成
int ReorderBuffer::Add(const RoBRow &row, Bus &bus)
{
    nextBuffer_.push(row);
    if (row.type == RoBType::registerWrite)
    {
        if (row.index != 0)
        {
            bus.getRegisterFile()[row.index].setDependency(nextBuffer_.getRealTail());//直接覆写寄存器依赖
        }
    }
    return nextBuffer_.getRealTail();
}

RoBRow &ReorderBuffer::operator[](const int &index) {return nextBuffer_[index];}

bool ReorderBuffer::full() {return buffer_.full();}

void ReorderBuffer::flush() {buffer_ = nextBuffer_;}

RoBRow &ReorderBuffer::getNewRow(const int &index) {return nextBuffer_[index];}

const RoBRow &ReorderBuffer::tellOldRow(const int &index) {return buffer_[index];}

void ReorderBuffer::clear() {nextBuffer_.clear();}

//TODO:确定buffer_以及nextbuffer的使用部分
void ReorderBuffer::tyrCommit(Bus &bus)
{
    if (buffer_.empty() || !buffer_.front().ready) return;
    auto tmp = buffer_.front();
    switch (tmp.type)
    {
        case RoBType::registerWrite:
        {//rowRoB.index = curInstruction.destinationRegister;
            //覆写寄存器数值并检查是否覆写dependency
            bus.getRegisterFile().writeRegister(tmp.index, tmp.value,  buffer_.getHead());
//            printDebugInfo(tmp, bus);
            break;
        }
        case RoBType::memoryWrite:
        {//rowRoB.index = bus.getLoadStoreBuffer().innerTailIndex();
            bus.getLoadStoreBuffer()[tmp.index].ready = true;
//            printDebugInfo(tmp, bus);
            break;
        }
        case RoBType::branch:
        {
            bool realBranch = static_cast<bool>(tmp.value);
            bus.getPredictor().update(tmp.address, realBranch);
            if (realBranch != tmp.branchAdvise)
            {
                bus.clear();
                bus.getInstructionManager().SetPC(tmp.index);
                return;
            }
            break;
            /*if (realBranch != tmp.branchAdvise)
            {
                tmp.tellAfterPC = nextBuffer_.front().tellAfterPC = tmp.index;
//                printDebugInfo(tmp, bus);
                bus.clear();
                bus.getInstructionManager().SetPC(tmp.index);
                return;//no need to nextBuffer_.pop();
            }
            else
            {
//                printDebugInfo(tmp, bus);
            }
            break;*/
        }
        case RoBType::end:
        {
            std::cout<<(static_cast<twoBite>(bus.getRegisterFile()[10].getValue()) & 255u)<<'\n';
            exit(0);
        }
    }
    nextBuffer_.pop();
}

/*void ReorderBuffer::printDebugInfo(RoBRow &row, Bus &bus) const
{
    row.info.print();
    bus.getRegisterFile().printRegisterFile();
    std::cout<<row.tellAfterPC<<'\n';
//    std::cout<<row.tellAfterPC<<" #"<<bus.getSourceManager().loadWord(4528)<<'\n';
}*/
