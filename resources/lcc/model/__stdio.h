#ifndef __STDIO_H
#define __STDIO_H



byte __onbreak() {
#asm
    RA	;test
    TEST 08
    JRZP lb__stdio__onbreak_end
    LIA 0xff
lb__stdio__onbreak_end:
#endasm

    return _reg_a;
}

/*; -----------------------------------------------
//; Wait BA ms
//; label prefix __sdtio15
//; -----------------------------------------------*/
ps_wait(word _var_ps_wait_nb) {
	load _var_ps_wait_nb;
#asm
	PUSH	; push A
lb__sdtio1501:
#ifdef __PC_RESONATOR_768K__
	WAIT 0xF0
#endif
#ifdef __PC_RESONATOR_576K__
	WAIT 0xB6
#endif
	LOOP lb__sdtio1501
	
	EXAB
	PUSH
lb__sdtio1502:
	LIA 0xFF
	PUSH
lb__sdtio1503:
#ifdef __PC_RESONATOR_768K__
	WAIT 0xF0
#endif
#ifdef __PC_RESONATOR_576K__
	WAIT 0xB6
#endif
	LOOP lb__sdtio1503
	LOOP lb__sdtio1502
#endasm
}

ps_disp_off() {
#asm
fct_ps_disp_off:
	LIP  95
	ANIM 0xFE
	OUTC
#endasm
}

ps_disp_on() {
#asm
fct_ps_disp_on:
	LIP  95
	ORIM 1
	OUTC
#endasm
}

byte xram ps_cur_x;
byte xram ps_cur_y;

/*; -----------------------------------------------
//; Writes a character in A to the display
//; label prefix 01
//; Used Registers: A,B,X,Y
//; label prefix __sdtio01
//; -----------------------------------------------*/
byte xram var_ps_putchar_con_rombank;
ps_putchar_con(char ps_putchar_con_c) {
	load ps_putchar_con_c;
	
#asm
fct_ps_putchar_con:
		
#ifdef __PC_IS_BANKSWITCHED__
	; if this is a bankswiched pocket, we
	; switch to the rom with the character table
	PUSH
	LIDP  __MEM_BANKSWITCH__
	LDD
	LIDP var_ps_putchar_con_rombank
	STD
	LIDP  __MEM_BANKSWITCH__
	LIA   __MEM_BS_IND_CHARTAB__
	STD
	POP
#endif
	
	CALL  fct_ps_ctabadr
	CALL  fct_ps_xcur2xgcur
	
	LP    REG_K
	EXAM
	
	LIA  __LCD_CHAR_WIDTH__-1
	PUSH
lb__sdtio0101:                       ; copy the 5 (or 6) pixmaps
	LIDP  ps_cur_y        ; y cursor into B
	LDD
	EXAB
	
	LP    REG_K
	LDM
	INCK
	CALL  fct_ps_dispadr
	
#ifdef __INVERS_CHARTAB__
	DXL
#endif
#ifndef __INVERS_CHARTAB__
	IXL
#endif
	DY
	; introduce here invert char flag
	; 255 -A -> A
		
	IYS 
	LOOP lb__sdtio0101
	
	LIDP  ps_cur_x        ; increase x cursor
	LDD
	ADIA  1
	STD
	
	CPIA  __DISP_CH_COLS__        ; if not max. column + 1,
	JRNZP lb__sdtio0102          ; then go on
	
	RA                    ; else: set to zero
	STD
	
	LIDP  ps_cur_y        ; ... and increase y
	LDD
	ADIA  1
	STD
	
	CPIA  __DISP_CH_ROWS__        ; if not max. row + 1,
	JRNZP  lb__sdtio0102         ; then go on
	
	CALL  fct_ps_scroll       ; else: scroll!
	
lb__sdtio0102:

#ifdef __PC_IS_BANKSWITCHED__
	; if this is a bankswiched pocket, we
	; restore to the initial rom bank
	PUSH
	LIDP var_ps_putchar_con_rombank
	LDD
	LIDP  __MEM_BANKSWITCH__
	STD
	POP
#endif
#endasm
}

/*; -----------------------------------------------
//; invert the display one line.
//; for pockets with only on line it is 
//; simple - just delete the display
//; (procedure isn't assembled then and
//; ps_scroll is the same as ps_clrscr
//; label prefix __sdtio07
//; -----------------------------------------------*/
ps_inv() {
	
#asm
fct_ps_inv:

	CALL fct_ps_disp_off
#if __PC_1350__ || __PC_1360__
	; copy L2C1 en L1C1
	; copy L3C1 en L2C1
	; copy L4C1 en L3C1
	; erase L4C1 
	LIB 0
	LIA 4
	PUSH
lb__sdtio0701:	
	
	
	LP REG_XH
	LIA		HB(__MEM_LCD_L1C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_XL
	LIA		LB(__MEM_LCD_L1C1__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L1C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_YL
	LIA		LB(__MEM_LCD_L1C1__-1)
	EXAM	
	LIA 59
	PUSH
	
lb__sdtio0702:
	IXL
	; 255 -A -> A
	LII 0xFF
	LP 0
	SBM
	LDM
	
	IYS
	LOOP lb__sdtio0702
	
	LP REG_B
	ADIM 2
	LOOP lb__sdtio0701

	LIB 0
	LIA 4
	PUSH
	lb__sdtio0703:	
	
	
	LP REG_XH
	LIA		HB(__MEM_LCD_L2C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_XL
	LIA		LB(__MEM_LCD_L2C1__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L2C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_YL
	LIA		LB(__MEM_LCD_L2C1__-1)
	EXAM	
	LIA 59
	PUSH
	
	lb__sdtio0704:
	IXL
	; 255 -A -> A
	LII 0xFF
	LP 0
	SBM
	LDM
	
	IYS
	LOOP lb__sdtio0704
	
	LP REG_B
	ADIM 2
	LOOP lb__sdtio0703
		
#endif

#if __PC_1262__
	; copy L2C1 en L1C1
	; copy L3C1 en L2C1
	; copy L4C1 en L3C1
	; erase L4C1 
	LIB 0
	LIA 2
	PUSH
lb__sdtio0701:	
	
	LP REG_XH
	LIA		HB(__MEM_LCD_L1C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_XL
	LIA		LB(__MEM_LCD_L1C1__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L1C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_YL
	LIA		LB(__MEM_LCD_L1C1__-1)
	EXAM	
	LIA 59
	PUSH
	
	lb__sdtio0702:
	IXL
	; 255 -A -> A
	LII 0xFF
	LP 0
	SBM
	LDM
	
	IYS
	LOOP lb__sdtio0702
	
	LP REG_B
	ADIM 8
	LOOP lb__sdtio0701	

	LIB 0
	LIA 2
	PUSH
	lb__sdtio0703:	
	
	LP REG_XH
	LIA		HB(__MEM_LCD_L2C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_XL
	LIA		LB(__MEM_LCD_L2C1__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L2C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_YL
	LIA		LB(__MEM_LCD_L2C1__-1)
	EXAM	
	LIA 59
	PUSH
	
	lb__sdtio0704:
	IXL
	; 255 -A -> A
	LII 0xFF
	LP 0
	SBM
	LDM
	
	IYS
	LOOP lb__sdtio0704
	
	LP REG_B
	ADIM 8
	LOOP lb__sdtio0703	
#endif

#if __PC_1251__

	LP REG_XH
	LIA		HB(__MEM_LCD_L1C1__-1)
	EXAM
	LP REG_XL
	LIA		LB(__MEM_LCD_L1C1__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L1C1__-1)
	EXAM
	LP REG_YL
	LIA		LB(__MEM_LCD_L1C1__-1)
	EXAM	
	LIA 59
	PUSH
	
	lb__sdtio0702:
	IXL
	; 255 -A -> A
	LII 0xFF
	LP 0
	SBM
	LDM
	
	IYS
	LOOP lb__sdtio0702
	
	LP REG_XH
	LIA		HB(__MEM_LCD_L1C2__-1)
	EXAM
	LP REG_XL
	LIA		LB(__MEM_LCD_L1C2__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L1C2__-1)
	EXAM
	LP REG_YL
	LIA		LB(__MEM_LCD_L1C2__-1)
	EXAM	
	LIA 59
	PUSH
	
	lb__sdtio0704:
	IXL
	; 255 -A -> A
	LII 0xFF
	LP 0
	SBM
	LDM
	
	IYS
	LOOP lb__sdtio0704

#endif
	CALL fct_ps_disp_on
#endasm
}


/*; -----------------------------------------------
//; Scrolls the display one line.
//; for pockets with only on line it is 
//; simple - just delete the display
//; (procedure isn't assembled then and
//; ps_scroll is the same as ps_clrscr
//; label prefix __sdtio05
//; -----------------------------------------------*/
ps_scroll() {
	
#asm
	fct_ps_scroll:
	
#if __PC_1350__ || __PC_1360__
	; copy L2C1 en L1C1
	; copy L3C1 en L2C1
	; copy L4C1 en L3C1
	; erase L4C1 
	LIB 0
	LIA 4
	PUSH
lb__sdtio0501:	
	
	
	LP REG_XH
	LIA		HB(__MEM_LCD_L2C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_XL
	LIA		LB(__MEM_LCD_L2C1__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L1C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_YL
	LIA		LB(__MEM_LCD_L1C1__-1)
	EXAM	
	LIA 29
	PUSH
	
lb__sdtio0502:
	IXL
	IYS
	LOOP lb__sdtio0502
	
	LP REG_B
	ADIM 2
	LOOP lb__sdtio0501

	; copy L2C1 en L1C1
	; copy L3C1 en L2C1
	; copy L4C1 en L3C1
	; erase L4C1 
	LIB 0
	LIA 4
	PUSH
lb__sdtio0503:	
	
	
	LP REG_XH
	LIA		HB(__MEM_LCD_L3C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_XL
	LIA		LB(__MEM_LCD_L3C1__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L2C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_YL
	LIA		LB(__MEM_LCD_L2C1__-1)
	EXAM	
	LIA 29
	PUSH
	
	lb__sdtio0504:
	IXL
	IYS
	LOOP lb__sdtio0504
	
	LP REG_B
	ADIM 2
	LOOP lb__sdtio0503

	; copy L2C1 en L1C1
	; copy L3C1 en L2C1
	; copy L4C1 en L3C1
	; erase L4C1 
	LIB 0
	LIA 4
	PUSH
	lb__sdtio0505:	
	
	
	LP REG_XH
	LIA		HB(__MEM_LCD_L4C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_XL
	LIA		LB(__MEM_LCD_L4C1__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L3C1__-1)
	EXAM
	EXAB
	ADM
	EXAB
	LP REG_YL
	LIA		LB(__MEM_LCD_L3C1__-1)
	EXAM	
	LIA 29
	PUSH
	
	lb__sdtio0506:
	IXL
	IYS
	LOOP lb__sdtio0506
	
	LP REG_B
	ADIM 2
	LOOP lb__sdtio0505



	; erase last row
	LIDP  __MEM_LCD_L4C1__
	LII  30-1
	RA
	FILD
	LIDP  __MEM_LCD_L4C2__
	LII  30-1
	FILD
	LIDP  __MEM_LCD_L4C3__
	LII  30-1
	FILD
	LIDP  __MEM_LCD_L4C4__
	LII  30-1
	FILD
	LIDP  __MEM_LCD_L4C5__
	LII  30-1
	FILD
#endif
	
#if __PC_1262__
	; copy L2C1 en L1C1
	; copy L2C2 en L1C2
	; erase L2C1 et L2C2
	LP REG_XH
	LIA		HB(__MEM_LCD_L2C1__-1)
	EXAM
	LP REG_XL
	LIA		LB(__MEM_LCD_L2C1__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L1C1__-1)
	EXAM
	LP REG_YL
	LIA		LB(__MEM_LCD_L1C1__-1)
	EXAM	
	LIA 60
	PUSH

lb__sdtio0501:
	IXL
	IYS
	LOOP lb__sdtio0501
	
	LP REG_XH
	LIA		HB(__MEM_LCD_L2C2__-1)
	EXAM
	LP REG_XL
	LIA		LB(__MEM_LCD_L2C2__-1)
	EXAM
	LP REG_YH
	LIA		HB(__MEM_LCD_L1C2__-1)
	EXAM
	LP REG_YL
	LIA		LB(__MEM_LCD_L1C2__-1)
	EXAM	
	LIA 60
	PUSH
	
	lb__sdtio0502:
	IXL
	IYS
	LOOP lb__sdtio0502	
	
	LIDP  __MEM_LCD_L2C1__
	LII  12*__LCD_CHAR_WIDTH__-1
	RA
	FILD
	
	LIDP __MEM_LCD_L2C2__
	LII  12*__LCD_CHAR_WIDTH__-1
	FILD
#endif
	
#if __PC_1251__
	CALL fct_ps_clrscr
#endif

	; ps_cur_x = 0
	; ps_cur_y = nb_rows -1
	LIDP  ps_cur_x        
	RA
	STD
	LIDP  ps_cur_y
	LIA  __DISP_CH_ROWS__-1
	STD
#endasm
}

//; -----------------------------------------------
//; Deletes all lines of the display
//; Used Registers: Various, for models which
//;   provide a suitable ROM call
//; label prefix 02
//; -----------------------------------------------
ps_clrscr() {
#asm
fct_ps_clrscr:
	
#if __PC_1251__
	
	; ensure, that LIDL can be uses instead of LIDP one time
	;.assume >MEM_LCD_PART1 - >MEM_LCD_PART2
	
	LIDP __MEM_LCD_L1C1__
	LII  12*__LCD_CHAR_WIDTH__-1
	RA
	FILD
	
	LIDL __MEM_LCD_L1C2__
	LII  12*__LCD_CHAR_WIDTH__-1
	FILD
#endif

#if __PC_1350__ || __PC_1360__
	
	CAL __INT_ROM_CLRSCR__
	CAL __INT_ROM_DISP__
#endif

#if __PC_1262__
	
	; aaa ensure, that LIDL can be uses instead of LIDP two times
	;.assume >MEM_LCD_L1C1 - >MEM_LCD_L2C1
	;.assume >MEM_LCD_L1C2 - >MEM_LCD_L2C2
	
	LIDP  __MEM_LCD_L1C1__
	LII  12*__LCD_CHAR_WIDTH__-1
	RA
	FILD
	
	LIDL LB(__MEM_LCD_L2C1__)
	LII  12*__LCD_CHAR_WIDTH__-1
	FILD
	
	LIDP  __MEM_LCD_L1C2__
	LII  12*__LCD_CHAR_WIDTH__-1
	FILD
	
	LIDL LB(__MEM_LCD_L2C2__)
	LII  12*__LCD_CHAR_WIDTH__-1
	FILD
	
#endif
	
	; other modells are not implemented yet
	;.error 1
	
	
	; now reset the cursors
	LIDP  ps_cur_x        
	RA
	STD
	LIDP  ps_cur_y
	STD

#endasm
}

/*; -----------------------------------------------
//; Converts x cursor position to graphics
//; cursor position
//;
//; Used Registers: A,B
//; Input value: A
//; Return value: A
//; label prefix __sdtio03
//; -----------------------------------------------*/
ps_xcur2xgcur(){
#asm	
fct_ps_xcur2xgcur:
	
#if __PC_1262__        
	LIDP ps_cur_x          ; Load x cursor position
	LDD                    ; into A and B
	EXAB
	LP   REG_B
	LDM
	SL                     ; Shift A two times and
	SL                     ; add B, this equals
	ADM                    ; multiply by five
	EXAB
#endif
	
#if __PC_1350__ || __PC_1360__
	
	LIDP ps_cur_x          ; Load left shifted x cursor
	LDD                    ; position into A and B
	SL
	EXAB
	LP   REG_B
	LDM
	SL                     ; Shift A one times and
	ADM                    ; add B, this equals
	EXAB                   ; multiply by six
	
#endif
	
	; other modells are not implemented yet
	;.error 1
	
#endasm
}

/*; -----------------------------------------------
//; Calculates the display adress of a
//; given graphics cursor location.
//
//; Positions that are to big to fit in
//; the display return the adress of the
//; most right display column if __RANGE_CHECK__
//; is on, otherwise the behaviour is undefined
//; (most likely crash).
//
//; Used Registers: A,B,Y
//; Input A (graph. cur. x), B (graph. cur. y)
//; Return value: Y
//; label prefix __sdtio04
//; -----------------------------------------------*/
ps_dispadr() {
#asm
fct_ps_dispadr:
	
#if __PC_1262__
	
	; -----------------------------------------------
	
	; First we handle the row
	PUSH
	RA
	LP   REG_B
	TSIM 0x01               ; Test for line 1
	JRZP lb__sdtio0407
	ORIA 0x40        
lb__sdtio0407: 
	LP   REG_YL
	EXAM
	POP
	
	; Now we handle the column block address
	PUSH
	
	CPIA 60			; If not char 13-24, jump ahead
	JRCP lb__sdtio0401
	LIA  HB(__MEM_LCD_L1C2__)
	LIB  60
	JRP  lb__sdtio0402		; "break"
	
	; Assert char position is in the range 1-12 
lb__sdtio0401:	
	LIA  HB(__MEM_LCD_L1C1__)
	LIB  0
	
	; Write result to YH
lb__sdtio0402:	
	LP REG_YH
	EXAM
	
	; Calculate graph x cur - block offset
	POP
	EXAB
	LP   REG_B
	SBM
	RA
	EXAB
	
	; Add it to Y
	LP   REG_YL
	ADB			; 16 bit add
	
	; -----------------------------------------------
	
#endif
	
#if __PC_1350__ || __PC_1360__

; -----------------------------------------------

; First we handle the row
	PUSH
	RA
	LP   REG_B
	TSIM 0x02               ; Test for line 3 and 4
	JRZP lb__sdtio0406
	LIA  0x1E
lb__sdtio0406: 
	TSIM 0x01               ; Test for line 1 and 2
	JRZP lb__sdtio0407
	ORIA 0x40        
lb__sdtio0407: 
	LP   REG_YL
	EXAM
	POP

	; Now we handle the column block address
	PUSH

	CPIA 120		; If not char 21-25, jump ahead
	JRCP lb__sdtio0401
	LIA  HB(__MEM_LCD_L1C5__)
	LIB  120
	JRP  lb__sdtio0405		; "break"

lb__sdtio0401:	
	CPIA 90			; If not char 16-20, jump ahead
	JRCP lb__sdtio0402
	LIA  HB(__MEM_LCD_L1C4__)
	LIB  90
	JRP  lb__sdtio0405		; "break"

lb__sdtio0402:	
	CPIA 60			; If not char 11-15, jump ahead
	JRCP lb__sdtio0403
	LIA  HB(__MEM_LCD_L1C3__)
	LIB  60
	JRP  lb__sdtio0405		; "break"

lb__sdtio0403:	
	CPIA 30			; If not char 6-10, jump ahead
	JRCP lb__sdtio0404
	LIA  HB(__MEM_LCD_L1C2__)
	LIB  30
	JRP  lb__sdtio0405		; "break"

	; Assert char position is in the range 1-5 
lb__sdtio0404:	
	LIA  HB(__MEM_LCD_L1C1__)
	LIB  0

	; Write result to YH
lb__sdtio0405:	
	LP REG_YH
	EXAM

	; Calculate graph x cur - block offset
	POP
	EXAB
	LP   REG_B
	SBM
	RA
	EXAB

	; Add it to Y
	LP   REG_YL
	ADB			; 16 bit add

; -----------------------------------------------
#endif
#endasm
}	

//; -----------------------------------------------
//; Calculates the address of a given character
//; (Register A) in the character table
//; The character stays in A
//;
//; Used Registers: A,B,X
//; Return value: X
//; label prefix __sdtio06
//; -----------------------------------------------
ps_ctabadr() {
	
#asm
fct_ps_ctabadr:
	LIB  1			; Default table offset: 0, plus on for IXL mnemonic
	
	
	PUSH			; Save A (= ascii code)
	
	LIA  HB(__MEM_CHARTAB__)       ; Table address -> X
	LP   REG_XH
	EXAM
	LIA  LB(__MEM_CHARTAB__)
	LP   REG_XL
	EXAM
	
	RA			; (00 B) -> BA = Table offset 
	EXAB
	
	SBB			; 16 bit sub
	DECP
	
	POP			; (00 ascii) -> BA			
	
	ADB			; 16 bit add
	DECP
	
	ADB			; 16 bit add
	DECP
	
	ADB			; 16 bit add
	DECP
	
	ADB			; 16 bit add
	DECP
	
	ADB			; 16 bit add
	DECP
	
#endasm
}

//; -----------------------------------------------
//; Display a byte pattern with address given in 
//; X+1, length in A and x pos in B
//; label prefix __sdtio14
//; -----------------------------------------------
ps_gprint() {
#asm
fct_ps_gprint:
	PUSH
	CALL  fct_ps_xcur2xgcur
	EXAB
	POP
fct_ps_gdraw:
	TSIA  0xff
	JRZP  lb__sdtio1402
	
	DECA
	PUSH			; counter for loop on stack
	
	EXAB			; x pos in K
	LP    8			;REG_K
	EXAM
	
lb__sdtio1401:                       ; copy n pixmaps
	LIDP  ps_cur_y        ; y cursor into B
	LDD
	EXAB
	
	LP    8			;REG_K
	LDM
	INCK
	CALL  fct_ps_dispadr
	IXL
	DY
	IYS 
	LOOP lb__sdtio1401
lb__sdtio1402:	
	RTN
	
	
ps_bitbltr:
	IXL
	DY
	IYS 
	RTN
	
ps_bitbltp:
	IXL
	DY
	IYS 
	RTN
#endasm
}


#endif // __STDIO_H
