#ifndef GRAPH_H
#define GRAPH_H

word xram _grfx_x1 at 0x6F63;
word xram _grfx_y1 at 0x6F65;
word xram _grfx_x2 at 0x6F67;
word xram _grfx_y2 at 0x6F69;

line(word x1,word y1, word x2, word y2) {
    _grfx_x1 = x1;
    _grfx_y1 = y1;
    _grfx_x2 = x2;
    _grfx_y2 = y2;
#save
#asm
    CALL    0xEAEB
#endasm
#restore
}

cls() {
#save
#asm
    CALL    0xDBA8
#endasm
#restore
}

#endif // GRAPH_H