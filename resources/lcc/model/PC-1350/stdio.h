#ifndef _STDIO_H
#define _STDIO_H

// implement putchar(int) and getchar()
// test for PC-1350
byte xram _lcd_dx at 0x7881;
byte xram _lcd_dy at 0x7880;

putchar(char c) {

//    if ( c=='\n') {
        #asm
        LIDP    0x788F
        ORID    0x01
        CALL    0xE983
        #endasm

        _lcd_dx++;
        if (_lcd_dx>23) {
            _lcd_dx = 0;
            _lcd_dy++;
        }
//    }
//    else {
//        _lcd_dx = 0;
//        _lcd_dy++;
//    }

    if (_lcd_dy >3) {
        //scroll up he screen
        #asm
        LIA     4
        CALL    0xE23C
        #endasm
        _lcd_dy = 3;
    }
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

