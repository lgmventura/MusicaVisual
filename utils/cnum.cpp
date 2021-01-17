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

unsigned char hexval(unsigned char c) // https://stackoverflow.com/questions/3790613/how-to-convert-a-string-of-hex-values-to-a-string
{
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    else if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    else abort();
}
void hex2ascii(const std::string& in, std::string& out) // https://stackoverflow.com/questions/3790613/how-to-convert-a-string-of-hex-values-to-a-string
{
    out.clear();
    out.reserve(in.length() / 2);
    for (std::string::const_iterator p = in.begin(); p != in.end(); p++)
    {
       unsigned char c = hexval(*p);
       p++;
       if (p == in.end()) break; // incomplete last digit - should report error
       c = (c << 4) + hexval(*p); // + takes precedence over <<
       out.push_back(c);
    }
}
