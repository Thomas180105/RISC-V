#ifndef RISC_V_QUEUE_H
#define RISC_V_QUEUE_H

#include <iostream>
using namespace std;

template <typename T, int maxSize>
class queue {
private:
    T data[maxSize];
    int head;
    int tail;

public:
    queue() : head(0), tail(0) {}

    //注意，默认queue的容量是一致的，这是由于这个赋值重载是为了flush函数的实现准备的
    queue &operator=(const queue &rhs)
    {
        if (this == &rhs) return *this;
        for (int i = 0; i < maxSize; ++i) data[i] = rhs.data[i];
        head = rhs.head, tail = rhs.tail;
        return *this;
    }

    queue &operator+(const queue &rhs)
    {
        if (this == &rhs) return *this;
        head = rhs.head;
        tail = rhs.tail;
        for (int i = 0; i < maxSize; ++i) data[i] = rhs.data[i];
    }

    ~queue() {}

    void push(T value)
    {
        data[tail] = value;
        tail = (tail + 1) % maxSize;
    }

    void pop() {head = (head + 1) % maxSize;}

    void clear() {head = tail = 0;};

    T &front() {return data[head];}

    T &operator[](const int index) {return data[index];}

    const T &operator[](const int index) const {return data[index];}

    int capacity() const {return maxSize;}

    int getInnerTail() const {return tail;}

    void setInnerTail(int newTail) {tail = newTail;}

    int getRealTail() const {return (tail - 1 + maxSize) % maxSize;}//返回实际存有值的最后一个下标

    int getHead() const {return head;}

    bool empty() const {return head == tail;}

    bool full() const {return (tail + 2) % maxSize == head;}//给一个余量
};


#endif //RISC_V_QUEUE_H
