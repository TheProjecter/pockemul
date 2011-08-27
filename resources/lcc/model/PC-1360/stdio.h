#ifndef STDIO_H
#define STDIO_H

// PC-1350

#include <__stdio.h>


// implement putchar(int) and getchar()
// test for PC-1350
byte xram _lcd_dx at 0x7881;
byte xram _lcd_dy at 0x7880;

putchar(char l_putchar_c) {

    if (_lcd_dy < 0) _lcd_dy = 0;

    if ( l_putchar_c!='\n') {
        load l_putchar_c;

        #save
        #asm
        LIDP    0x788F
        ORID    0x01
        CALL    0xE983
        #endasm
        #restore
        _lcd_dx++;
        if (_lcd_dx>23) {
            _lcd_dx = 0;
            _lcd_dy++;
        }
    }
    else {
        _lcd_dx = 0;
        _lcd_dy++;
    }

    if (_lcd_dy >3) {
        //scroll up the screen
        #save
        #asm
        LIA     4
        CALL    0xE23C
        #endasm
        #restore
        _lcd_dy = 3;
    }
}

char xram l_getchar_ret;
char getchar() {

#save
#asm

    CALL 0x0436

    JRCP lb_getchar_key
    RA
    JRP lb_getchar_ascii
lb_getchar_key:
    ADIA  0x03
    LIB   0x84
    CALL 0x0297
    IXL
lb_getchar_ascii:
    LIDP l_getchar_ret
    STD
#endasm

#restore
    return l_getchar_ret;
}

byte onbreak() {

    return __onbreak();
}

#endif // _STDIO_H

