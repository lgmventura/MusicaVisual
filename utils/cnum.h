#ifndef CNUM_H
#define CNUM_H

#include <stdlib.h>
#include <limits>
#include <string>

short f2short_safe(float a);
int f2int_safe(float a);
int floor1(int a);

unsigned char hexval(unsigned char c);
void hex2ascii(const std::string& in, std::string& out);

#endif // CNUM_H
