#include "load_store_buffer.h"
#include "bus.h"
loadStoreRow &loadStoreBuffer::operator[](Location index) {return buffer_[index];}

int loadStoreBuffer::innerTailIndex() {return buffer_.getInnerTail();}

bool loadStoreBuffer::Full() const {return buffer_.full();}

//不能简单地一窝端，如果LSB头部有已经ready，正在等待RoB反馈的指令，这些指令应当被保存
void loadStoreBuffer::clear()
{
    for (int i = buffer_.getHead(), c = buffer_.capacity(), t = buffer_.getInnerTail(); i != t; i = (i + 1) % c)
    {
        auto tmp = buffer_[i];
        if ((tmp.type == InstructionType::SB || tmp.type == InstructionType::SH || tmp.type == InstructionType::SW) && tmp.ready) continue;
        buffer_.setInnerTail(i);
        break;
    }
    if (buffer_.empty()) clock_LSB = 0;//important!!!
}

//需要确保广播的时候已经将tmp.location同步更改好了
void loadStoreBuffer::Run(Bus &bus)
{
    //ready标志被在RoB中更新
    if (clock_LSB)
    {
        if (!--clock_LSB)
        {
            loadStoreRow tmp = buffer_.front();
            switch (tmp.type)
            {
                case InstructionType::LW:
                    bus.getReorderBuffer()[tmp.RoBIndex].ready = true;
                    bus.getReorderBuffer()[tmp.RoBIndex].value = bus.getSourceManager().loadWord(tmp.offset + tmp.location);
                    break;
                case InstructionType::LH:
                    bus.getReorderBuffer()[tmp.RoBIndex].ready = true;
                    bus.getReorderBuffer()[tmp.RoBIndex].value = bus.getSourceManager().loadHalfWord_signed(tmp.offset + tmp.location);
                    break;
                case InstructionType::LB:
                    bus.getReorderBuffer()[tmp.RoBIndex].ready = true;
                    bus.getReorderBuffer()[tmp.RoBIndex].value = bus.getSourceManager().loadByte_signed(tmp.offset + tmp.location);
                    break;
                case InstructionType::LHU:
                    bus.getReorderBuffer()[tmp.RoBIndex].ready = true;
                    bus.getReorderBuffer()[tmp.RoBIndex].value = bus.getSourceManager().loadHalfWord(tmp.offset + tmp.location);
                    break;
                case InstructionType::LBU:
                    bus.getReorderBuffer()[tmp.RoBIndex].ready = true;
                    bus.getReorderBuffer()[tmp.RoBIndex].value = bus.getSourceManager().loadByte(tmp.offset + tmp.location);
                    break;
                case InstructionType::SW:
                    bus.getSourceManager().storeWord(tmp.location + tmp.offset, tmp.value);
                    break;
                case InstructionType::SH:
                    bus.getSourceManager().storeHalfWord(tmp.location + tmp.offset, static_cast<twoBite>(tmp.value));
                    break;
                case InstructionType::SB:
                    bus.getSourceManager().storeByte(tmp.location + tmp.offset, static_cast<Bite>(tmp.value));
                    break;
            }
            buffer_.pop();
        }
    }
    else if (!buffer_.empty() && buffer_.front().ready) clock_LSB = 2;

    updateRoBTag(bus.getReorderBuffer());
}

void loadStoreBuffer::updateRoBTag(ReorderBuffer &reorderBuffer)
{
    for (int i = buffer_.getHead(), t = buffer_.getInnerTail(), c = buffer_.capacity(); i != t; i = (i + 1) % c)
    {
        loadStoreRow tmp = buffer_[i];
        if (tmp.locationDependence && reorderBuffer[tmp.locationQ1].ready)
        {
            tmp.location = reorderBuffer[tmp.locationQ1].value;
            tmp.locationDependence = false;
        }
        if (tmp.valueDependence && reorderBuffer[tmp.valueQ2].ready)//事实上只有可能SB SH SW可能可以走进来
        {
            tmp.value = reorderBuffer[tmp.valueQ2].value;
            tmp.valueDependence = false;
        }
        if (tmp.type == InstructionType::SB || tmp.type == InstructionType::SH || tmp.type == InstructionType::SW)//memoryWrite,需要先交给RoB一个ready的信号，等待RoB发射ready的信号才可真正开始写内存
        {
            if (!tmp.valueDependence && !tmp.locationDependence && !tmp.ready) reorderBuffer.getNewRow(tmp.RoBIndex).ready = true;
        }
        else//Load
        {
            if (!tmp.valueDependence && !tmp.locationDependence) tmp.ready = true;
        }
        buffer_[i] = tmp;
    }
}

void loadStoreBuffer::Add(const loadStoreRow &row, Bus &bus) {buffer_.push(row);}
