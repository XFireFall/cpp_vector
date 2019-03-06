#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "lib/vector.hpp"

int main()
{
    logCheck(nullptr);
    Vector a(5), c(5);
    c[0] = 4;
    c[3] = c[0];
    a = c;
    c.Dump();
    a += c;
    a.Dump();
    return 0;
}
