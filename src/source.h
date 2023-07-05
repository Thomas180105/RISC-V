#ifndef RISC_V_SOURCE_H
#define RISC_V_SOURCE_H
#include "type.h"

class sourceManager{
public:
    Bite * memory;
public:
    explicit sourceManager(int size)
    {
        memory = new Bite [size];
        sourceInput();
    }
    ~sourceManager() {delete [] memory;}
    void sourceInput();
    Context readInstruction(Context loc);

//    void printInstruction(Context loc);

    Context loadWord(Location index) const;
    Context loadHalfWord(Location index) const;
    Context loadHalfWord_signed(Location index) const;
    Context loadByte(Location index) const;
    Context loadByte_signed(Location index) const;

    void storeWord(Location index, Context num);
    void storeHalfWord(Location index, twoBite num);
    void storeByte(Location index, Bite num);
};


#endif //RISC_V_SOURCE_H
