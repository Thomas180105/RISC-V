#ifndef RISC_V_BUS_H
#define RISC_V_BUS_H
#include "Instruction.h"
#include "load_store_buffer.h"
#include "queue.h"
#include "registerFile.h"
#include "ReorderBuffer.h"
#include "reservationStation.h"
#include "predictor.h"
#include "source.h"
#include "type.h"

class Bus {
private:
    InstructionManager instructionManager_;
    loadStoreBuffer loadStoreBuffer_;
    registerFile registerFile_;
    ReorderBuffer ReorderBuffer_;
    ReservationStation reservationStation_;
    sourceManager sourceManager_;
    Predictor predictor_;
    long clock_ = 0;
public:
    Bus() : instructionManager_(), loadStoreBuffer_(), registerFile_(), ReorderBuffer_(), reservationStation_(), sourceManager_(204800) {}
    InstructionManager &getInstructionManager() {return instructionManager_;}
    loadStoreBuffer &getLoadStoreBuffer() {return loadStoreBuffer_;}
    registerFile &getRegisterFile() {return registerFile_;}
    ReservationStation &getReservationStation() {return reservationStation_;}
    sourceManager &getSourceManager() {return sourceManager_;}
    ReorderBuffer &getReorderBuffer() {return ReorderBuffer_;}
    Predictor &getPredictor() {return predictor_;}

    void clear();
    void flush();

    void Run();
    long long clock() const{return clock_;}
};


#endif //RISC_V_BUS_H
