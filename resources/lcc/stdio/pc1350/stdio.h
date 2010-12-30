// implement putchar(int) and getchar()
// test for PC-1350


putchar(char c) {

    byte xram dx at 0x7881;
    byte xram dy at 0x7880;

    #asm
    CALL    0xE983
    #endasm

    dx++;
    if (dx>23) {
        dx = 0;
        dy++;
    }
    if (dy >3) {
        //scroll up he screen
        #asm
            LIA     4
            CALL    0xE23C
        #endasm
        dy = 3;
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


main() {
    char k;

    k= getchar();
    putchar(k);
}
