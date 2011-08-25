#ifndef __STDIO_H
#define __STDIO_H


byte var__stdio_reg_a at 2;

byte __onbreak() {
#asm
    RA
    TEST 08
    JRZP lb__stdio__onbreak_end
    LIA 0xff
lb__stdio__onbreak_end:
#endasm
    return var__stdio_reg_a;
}

#endif // __STDIO_H
