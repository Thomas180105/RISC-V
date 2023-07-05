#ifndef RISC_V_REGISTERFILE_H
#define RISC_V_REGISTERFILE_H
#include "type.h"
#include "queue.h"
class Register{
private:
    Context value_ = 0;
    bool occupy_ = false;
    int dependency_ = 0;
public:
    void setDependency(int index);

    void tryResetDependency(int index);

    void clearOccupy();

    Register &operator+=(const signedContext &rhs);

    Context &getValue();

    bool tellOccupy() const;

    int tellDependency() const;
};

class registerFile {
public:
    const static int maxSize = 32;
private:
    Register buffer_[maxSize];
    Register nextBuffer_[maxSize];
//    queue<Register, maxSize> buffer_;
//    queue<Register, maxSize> nextBuffer_;
public:
    void flush();
    void writeRegister(int index, Context value, int dependency_);
    void modifyDependency(int index, int dependency_);
    void clearAllOccupy();
    Register &operator[](int index);
    const Register &getOldRegister(int index) const;
    void printRegisterFile();
};


#endif //RISC_V_REGISTERFILE_H
