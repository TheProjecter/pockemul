#ifndef STDIO_H
#define STDIO_H

// PC-1251

#include <__stdio.h>

/*
Chartable commence en 0x4464
5 byte par char
dans l'ordre

disp ram F800 -> F83B    F87B <- F840

*/




byte onbreak() {

    return __onbreak();
}

#endif // _STDIO_H
