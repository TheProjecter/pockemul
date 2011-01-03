#ifndef _STDIO_H
#define _STDIO_H

// implement putchar(int) and getchar()
// test for PC-1350


putchar(char c) {

    #asm

    CALL    0xE983
    #endasm

}

char getchar() {

#asm

    CALL 0x0436

    JPC getchar_key:
    RA
getchar_key:
    RTN

#endasm

}

#endif // _STDIO_H

