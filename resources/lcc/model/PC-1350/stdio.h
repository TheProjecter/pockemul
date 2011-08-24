#ifndef _STDIO_H
#define _STDIO_H

// implement putchar(int) and getchar()
// test for PC-1350
byte xram _lcd_dx at 0x7881;
byte xram _lcd_dy at 0x7880;
byte _reg_a at 2;

putchar(char c) {

    if (_lcd_dy < 0) _lcd_dy = 0;

    if ( c!='\n') {
        load c;

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


char getchar() {
#save
#asm

    CALL 0x0436

    JPC getchar_key:
    RA
getchar_key:

#endasm
#restore
    return _reg_a;
}

byte onbreak() {
#asm
    RA
    TEST 08
    JRZP onbreak_end
    LIA 0xff
onbreak_end:
#endasm
    return _reg_a;
}

#endif // _STDIO_H

