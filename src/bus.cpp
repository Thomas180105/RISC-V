#include "bus.h"

void Bus::clear()
{
    getInstructionManager().clear();
    getLoadStoreBuffer().clear();
    getRegisterFile().clearAllOccupy();
    getReservationStation().clear();
    getReorderBuffer().clear();
}

void Bus::Run()
{
    while(true)
    {
        loadStoreBuffer_.Run(*this);
        instructionManager_.launch(*this);
        reservationStation_.run(ReorderBuffer_);
        ReorderBuffer_.tyrCommit(*this);
        ++clock_;
        flush();
    }
}

void Bus::flush()
{
    //TODO:LSB的flush体系没有实现
    getRegisterFile().flush();
    getReservationStation().flush();
    getReorderBuffer().flush();
}
