#ifndef VECTOR_HPP_INCLUDED
#define VECTOR_HPP_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "memcheck.hpp"

typedef int vector_t;

class Vector
{
private:
    int capacity_;
    vector_t* data_;

public:
    Vector();
    Vector(int cap);
    Vector(const Vector& that);
    ~Vector();

    void Swap(Vector& that);
    void Dump();

    vector_t& operator[](int idx);

    const Vector& operator=(const Vector& that);
    const Vector& operator+=(Vector& that);
};

Vector::Vector():
    capacity_(0),
    data_(nullptr)
{ }

Vector::Vector(int cap):
    capacity_(cap),
    data_(new vector_t[capacity_] {})
{ }

Vector:: Vector(const Vector& that):
    capacity_(that.capacity_),
    data_(new vector_t[capacity_] {})
{
    for(int i = 0; i < capacity_; i++)
        data_[i] = that.data_[i];
}

Vector::~Vector()
{
    capacity_ = 0;
    delete[] data_;
    data_ = nullptr;
}

void Vector::Swap(Vector& that)
{
    std::swap(capacity_, that.capacity_);
    std::swap(data_,     that.data_);
    return;
}

void Vector::Dump()
{
    printf("Vector at [%p]:\n{\n", this);

    printf("\tcapacity = %d\n", capacity_);

    printf("\tdata at [%p]:\n", data_);
    {
        if(capacity_ == 0)
        {
            puts("empty");
            return;
        }

        int width = 0;
        for(int i = capacity_ - 1; i > 0; i /= 10)
            width++;

        puts("\t{");
        for(int i = 0; i < capacity_; i++)
        {
            printf("\t\t[%*d]: ", width, i);
            std::cout << data_[i] << "\n";
        }
        puts("\t}");
    }
    puts("}");

    return;
}

vector_t& Vector::operator[](int idx)
{
    return data_[idx];
}

const Vector& Vector::operator=(const Vector& that)
{
    Vector tmp(that);
    Swap(tmp);
    return *this;
}

const Vector& Vector::operator+=(Vector& that)
{
    Vector tmp(capacity_ + that.capacity_);
    for(int i = 0; i < capacity_; i++)
        tmp.data_[i] = data_[i];
    for(int i = 0; i < that.capacity_; i++)
        tmp.data_[capacity_ + i] = that.data_[i];
    Swap(tmp);
    return *this;
}

#endif // VECTOR_HPP_INCLUDED
