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

byte xram _lcd_x;
byte xram _lcd_y;

setcurpos(byte setpos_x,byte setpos_y) {
    _lcd_x = setpos_x;
    _lcd_y = setpos_y;
}

byte xram _chartable1 at 0x4464;
byte xram _chartable2 at 0x4564;
byte xram _chartable3 at 0x4664;
byte xram _chartable4 at 0x4764;
byte xram _chartable5 at 0x4864;

byte xram _disp_ram at 0xF800;

byte _grxcalc_dispadr()  {
    if (_lcd_x<=0x3B) {
        return _lcd_x;
    }
    else {
        return (0xBB - _lcd_x);
    }
}

drawchar(char _drawchar_c) {
    for (i=0;i<5;i++) {
        switch (_drawchar_c / 5) {
            0 : b =_chartable1[5*_drawchar_c+1];
        }

        if (_drawchar_c <= 51) _adr = _drawchar_c;
        else if (_drawchar_c <= 51) _adr = _drawchar_c;
        _disp_ram[_grxcalc_dispadr()] = _chartable[5*_drawchar_c+i];
        _lcd_x++;
    }
    if (_lcd_x > 119 ) _lcd_x = 0;
}

byte onbreak() {

    return __onbreak();
}

#endif // _STDIO_H
