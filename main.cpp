#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "lib/vector.hpp"

int main()
{
    logCheck(nullptr);
    Vector a(5), c(2);
    c[0] = -2;
    a[-1] = -c[0] + 1;
    a += c;
    a.Dump();
    c.Dump();
    a += c;
    a.Dump();
    c.Dump();
    c = a;
    c.Push(42);
    c.Push(43);
    c.Dump();

    c.Pop();
    c.Pop();
    c.Pop();
    c.Dump();
    c.Pop();
    c.Pop();
    c.Clear();
    c.Dump();

    c.Pop();
    c.Pop();
    c.Pop();
    c.Dump();
    return 0;
}
