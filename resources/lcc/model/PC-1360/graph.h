//TODO add screen inverter
#ifndef GRAPH_H
#define GRAPH_H

byte xram _grfx_graph at 0x6F62;
word xram _grfx_x1 at 0x6F63;
word xram _grfx_y1 at 0x6F65;
word xram _grfx_x2 at 0x6F67;
word xram _grfx_y2 at 0x6F69;

#define GSET    1
#define GRESET  2
#define GINV    4
#define GBOX    8
#define GBOXF   16

/*! 
 \brief Draw a Line from (x1,y1) to (x2,y2)
 The lineflag can take those values :
 - GSET : Pixels are set
 - GRESET : Pixels are reset
 - GINV : Pixels are inverted
 - GBOX : A box is drawn
 - GBOXF : A filled box is drawn
 
 \fn line 

 \param x1 (0-149)
 \param y1 (0-31)
 \param x2 
 \param y2 
 \param lineflag 
*/
line(word x1,word y1, word x2, word y2,byte lineflag) {
    _grfx_x1 = x1;
    _grfx_y1 = y1;
    _grfx_x2 = x2;
    _grfx_y2 = y2;

    _grfx_graph = lineflag;
#save
#asm
    LIJ     01
    CALL    0xEAEB
#endasm
#restore
}

/*! 
 \brief Draw pixel on (x,y)
  The psetflag can take those values :
  - GSET : Pixels are set
  - GRESET : Pixels are reset
  - GINV : Pixels are inverted

 \fn pset 
 \param px1 
 \param py1 
 \param psetflag 
*/
pset(word px1,word py1,byte psetflag) {
    _grfx_x1 = px1;
    _grfx_y1 = py1;

    _grfx_graph = psetflag;

#save
#asm
    LIJ     01
    CALL    0xEF24
#endasm
#restore
}

/*! 
 \brief Clear the screen
 
 \fn cls 
*/
cls() {
    _grfx_graph = 1;
#save
#asm
    LIJ     01
    CALL    0xDBA8
#endasm
#restore
}

#endif // GRAPH_H
