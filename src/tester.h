#ifndef RISC_V_TESTER_H
#define RISC_V_TESTER_H

#include<iostream>
#include <bitset>
#include "type.h"
#include "Instruction.h"
#include "Instruction.cpp"
using namespace std;

void printNum(Context n)
{
    for (int i = 7; i >= 0; --i)
    {
        cout<<"#"<<4 * i + 3<<"~"<<4 * i<<" : ";
        for (int j = 3; j >= 0; --j) cout<<((n >> (4 * i + j)) & 1);
        cout<<'\n';
    }
}

//测试了切分指令的相关函数，暂时没有发现bug
void testerOfInstruction()
{
    Context num = 0b11100001101111001101100010110001;
    freopen("input", "w", stdout);
    std::cout<<bitset<32>(num)<<'\n';
    printNum(num);

    freopen("product", "w", stdout);
    Context product = getTop7(num);
    std::cout<<bitset<32>(product)<<'\n';
    printNum(product);
}

class Identify{
private:
    int data[32];
    int nextData[32];
public:
    Identify() {}
    ~Identify() {}
    int &operator[](const int index)
    {
        std::cout<<"now we are using the int &, the index is "<<index<<'\n';
        return data[index];
    }
    const int &operator[](const int index) const
    {
        std::cout<<"now we are using the const int &, the index is "<<index<<'\n';
        return data[index];
    }
};

void testOfIdentify()
{
    Identify myTest;
    myTest[1] = 8;
    myTest[2] = myTest[1];
    int tmp = myTest[2];
    std::cout<<tmp;
}

#endif //RISC_V_TESTER_H
