#include "registerFile.h"
void Register::setDependency(int newDependency_)
{
    dependency_ = newDependency_;
    occupy_ = true;
}

void Register::tryResetDependency(int index)
{
    if (dependency_ == index) occupy_ = false;
}

void Register::clearOccupy()
{
    occupy_ = false;
}

Register &Register::operator+=(const signedContext &rhs)
{
    value_ += rhs;
    return *this;
}

Context &Register::getValue()
{
    return value_;
}

bool Register::tellOccupy() const
{
    return occupy_;
}

int Register::tellDependency() const
{
    return dependency_;
}

void registerFile::flush()
{
    for (int i = 0; i < maxSize; ++i)
    {
        buffer_[i] = nextBuffer_[i];
    }
}

void registerFile::writeRegister(int index, Context value, int inputDependency_)
{
    if (index != 0)
    {
        nextBuffer_[index].tryResetDependency(inputDependency_);
        nextBuffer_[index].getValue() = value;
    }
}

void registerFile::modifyDependency(int index, int inputDependency_)
{
    nextBuffer_[index].setDependency(inputDependency_);
}

Register &registerFile::operator[](const int index)
{
    return nextBuffer_[index];
}

const Register &registerFile::getOldRegister(const int index) const
{
    return buffer_[index];
}
void registerFile::clearAllOccupy()
{
    for (auto & i : nextBuffer_)
    {
        i.clearOccupy();
    }
 }

void registerFile::printRegisterFile()//输出最新的信息
{
    for (auto & i : nextBuffer_)
    {
        std::cout<<i.getValue()<<" ";
    }
}




