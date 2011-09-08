#ifndef STRING_H
#define STRING_H

#include <stdio.h>

//; -----------------------------------------------
//; Converts a word given in BA to a string
//; buffer pointed to by Y+1. There is NO trailing
//; ascii zero character writen after the string
//; Used registers: A,B,Y,I,K,L,M,N,0x0C .. 0x10
//; label prefix __sdtio20
//; -----------------------------------------------

ps_word2str(word value,char xram *s1) {

#save	
#asm
	POP
	LP REG_YH
	EXAM
	INCP
	POP
	EXAM
	DY
	POP
	EXAB
	POP
fct_ps_word2str:
	PUSH			; save low byte on stack
	LII	0x07		; I is counter for 7 bits
	RA			; 0 => A
	LP	REG_L		; Clear internal RAM from L to 0x10
	FILM			; The BCD buffer is 0x0E 0x0F 0x10
lb__sdtio2002:	
	DECB			; B-1 => B (decrement high byte)
	JRCP	lb__sdtio2001		; If end of loop, jump ahead
	LIA	0x56		; 0x56 => A
	LP	0x10		; Last position of BCD buffer => P
	ADN			; BCD add 0x56 B times
	LIA	0x02		; 2 => A
	LP	0x0F		; Last position-1 of BCD buffer => P
	ADN			; BCD add = add 200
	JRM	lb__sdtio2002		; Loop jump
lb__sdtio2001:	
	POP			; Load low byte from stack
	EXAB			; A <=> B
lb__sdtio2004:	
	DECB			; B-1 => B
	JRCP	lb__sdtio2003		; If end of loop, jump ahead
	LIA	0x01		; 1 => A
	LP	0x10		; Last position of BCD buffer => P
	ADN			; BCD add 1 B times
	JRM	lb__sdtio2004		; Loop jump
lb__sdtio2003:	
	LP	REG_K		; 5 => K; counter for max. 5 BCD
	LIA	0x05		; digits
	EXAM
lb__sdtio2007:	
	INCL			; Set L (Flag for leading zeros)
	RA			; 0 => A
	LP	0x0E		; First position of BCD buffer => A
	EXAM
	ADIA	0x30		; Add ASCII offset
	DECL			; L-1 => L; If L != 0 (no leading
	JRNZP	lb__sdtio2005		; zero), jump ahead
	CPIA	0x30		; If ASCII-"0", jump ahead
	JRZP	lb__sdtio2006
	INCL			; Inc. L, so it will never be zero
lb__sdtio2005:	
	IYS			; Write character to memory location
lb__sdtio2006:	
	LII	0x02		; Shift the three byte long BCD buffer
	LP	0x10		; on nibble to the left
	SLW
	DECK			; K-1 => K
	JRNZM	lb__sdtio2007		; Loop until last BCD digit written
	
#endasm
#restore
}


/*! 
	\brief Writes a zero terminated string
	
	\fn putstr 
	\param t  string pointer
*/
ps_putstr(char xram * t) {
	char c;
	
	c = *t;
	while (c>0) {
		ps_putchar_con(c);
		t++;
		c = *t;
	}
}


/*! 
 \brief return the lenght of a string
 
 \fn ps_strlen 
 \param t1 
 \return byte 
*/
byte ps_strlen(char xram * s) {
	byte _len;
	_len = 0;
	while (s[_len]>0){ _len++;}
	return _len;
}


/* reverse:  reverse string s in place */
ps_reverse(char xram * s)
{
	byte i, j;
	char c,d;
	
	j = ps_strlen(s) - 1;
	
	for (i = 0; i<j; ) {
		c = s[i];
		d = s[j];
		s[i] = d;
		s[j] = c;
		i++; j--
	}
}

/*! 
 \brief convert a byte to a string
 
 \fn btoa 
 \param v 
 \param _str 
 \param radix between 1 and 36
 \return byte error code
*/
byte btoa(byte v,char xram *_str,byte radix) {
	byte i;
	
	//	if ((radix > 36) || (radix <= 1))
	//	{
	//		return 0;
	//	}
	char xram *tmp;
	tmp = _str;
	i=0;
	while (v>0)
	{
		i = v % radix;
		v = v / radix;
		if (i < 10) {
			*_str = 48+i;
			_str++;
		}
		else {
			*_str = i + 65 - 10;
			_str++;
		}
	}
	*_str = 0;
	ps_reverse(tmp);
	
	return 1;	
}


#endif
