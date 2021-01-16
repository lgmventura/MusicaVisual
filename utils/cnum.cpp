/*
 * Useful tools for using numbers in C - safe conversion and constants
 * */

#include "cnum.h"


const short SHORT_MAX = std::numeric_limits<short>::max();// = 32767;
const short SHORT_MIN = std::numeric_limits<short>::min();// = -32768;
short f2short_safe(float a)
{
    if (a > SHORT_MAX)
        return SHORT_MAX;
    else if ( a < SHORT_MIN)
        return SHORT_MIN;
    else
        return (short)(a + 0.5);
}
const int INT_MAX = std::numeric_limits<int>::max();// =  2147483647;
const int INT_MIN = std::numeric_limits<int>::min();// = -2147483648;
int f2int_safe(float a)
{
    if (a > INT_MAX)
        return INT_MAX;
    else if ( a < INT_MIN)
        return INT_MIN;
    else
        return (int)(a + 0.5);
}

int floor1(int a)
{
    if (a < 1)
        return 1;
    else
        return a;
}
