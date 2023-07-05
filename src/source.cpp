#include "source.h"
#include "type.h"
//#define testSource
using std::string;
void sourceManager::sourceInput()
{
#ifdef testSource
    string cur;
    int pos = 0;
    while(std::cin>>cur && cur != "%%%")
    {
        if (cur[0] == '@') pos = stoi(cur.substr(1), nullptr, 16);
        else
        {
            memory[pos++] = static_cast<Bite>(stoi(cur, nullptr, 16));
        }
    }
    std::cout<<"the input is taken\n";
    for (int i = 0, tmp; i < 10; ++i)
    {
        std::cin>>std::dec>>tmp;
        printInstruction(tmp);
    }
#else
    string cur;
    int pos = 0;
    while(std::cin>>cur)
    {
        if (cur[0] == '@') pos = stoi(cur.substr(1), nullptr, 16);
        else
        {
            memory[pos++] = static_cast<Bite>(stoi(cur, nullptr, 16));
        }
    }
#endif
}

#include <bitset>
#include "Instruction.h"
/*void sourceManager::printInstruction(Context loc)
{
    Context tmp = *(reinterpret_cast<Context *>(memory + loc));
    std::cout<<std::hex<<tmp<<" ";
    std::cout<<std::bitset<32>(tmp)<<' '<<std::dec;
    getInstructionInfo(tmp).print();
}*/

Context sourceManager::readInstruction(Context loc)
{
    return *(reinterpret_cast<Context *>(memory + loc));
}

Context sourceManager::loadWord(Location index) const {return *(reinterpret_cast<Context*>(memory + index));}

Context sourceManager::loadHalfWord(Location index) const {return static_cast<Context>(*(reinterpret_cast<twoBite*>(memory + index)));}

Context sourceManager::loadHalfWord_signed(Location index) const {return static_cast<Context>(*reinterpret_cast<signedTwoBite*>(memory + index));}

Context sourceManager::loadByte(Location index) const {return static_cast<Context>(*(memory + index));}

Context sourceManager::loadByte_signed(Location index) const {return static_cast<Context>(*reinterpret_cast<signedBite*>(memory + index));}

void sourceManager::storeWord(Location index, Context num) {*(reinterpret_cast<Context*>(memory + index)) = num;}

void sourceManager::storeHalfWord(Location index, twoBite num) {*(reinterpret_cast<twoBite*>(memory + index)) = num;}

void sourceManager::storeByte(Location index, Bite num) {memory[index] = num;}

//写法参考lyy学长