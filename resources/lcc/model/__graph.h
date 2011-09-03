#ifndef __GRAPH_H
#define __GRAPH_H

#include <__stdio.h>

#define GSET    1
#define GRESET  2
#define GINV    4
#define GBOX    8
#define GBOXF   16

/*! 
 \brief Turn the corresponding pixel On/Off depending of Flag
 Flag can take 2 values : GSET or GRESET (corresponding values 0,1)
 
 \fn ps_pset 
 \param v_ps_pset_x 
 \param v_ps_pset_y 
 \param v_ps_pset_flag 
*/
ps_pset(byte v_ps_pset_x, byte v_ps_pset_y,byte v_ps_pset_flag) {
	
//	if (v_ps_pset_x >= __DISP_GR_COLS__) return;
//	if (v_ps_pset_y >= __DISP_GR_ROWS__) return;
	_reg_m = v_ps_pset_x;
	_reg_n = v_ps_pset_y;// / 8;
//	_reg_k = v_ps_pset_y - (_reg_n * 8);
	_reg_j = v_ps_pset_flag;
	
	_LCC_DEPEND(ps_dispadr);	
	
#asm
fct_ps_pset:
	LP REG_M
	CPIM __DISP_GR_COLS__ - 1
	JRCP l_ps_pset101	
	RTN
l_ps_pset101:		; OK x < __DISP_GR_COLS__
	LP REG_N
	CPIM __DISP_GR_ROWS__ - 1
	JRCP l_ps_pset102	
	RTN
l_ps_pset102:		; OK y < __DISP_GR_ROWS__
	LDM				; N -> A
	RC
	SR
	RC
	SR
	RC
	SR
	RC				; N /8
	PUSH			; PUSH N/8
	RC
	SL
	RC
	SL
	RC
	SL
	RC				; A = 8* int(N/8)
	SBM				; N = N - A
	LDM				; A = N
	LP REG_K		;
	EXAM			; Store result to K
	LP REG_N
	POP				;
	EXAM

	LP REG_N
	LDM
	EXAB
	LP REG_M
	LDM
	CALL fct_ps_dispadr			; Input A (graph. cur. x), B (graph. cur. y)
								; Return value: Y
	DY
	IY
	LDD							; current (DP) -> A
	LP REG_L
	EXAM						; current display column in L
	LP REG_K
	LDM							; mod8 in K
	PUSH
	LIA 1
	SR
l_ps_pset01:
	SL
	LOOP l_ps_pset01			; A contient new value to OR	

	LP REG_J
	CPIM 1	
	JRZP l_ps_pset02			; Set
	
	LII 0xFF					; Reset
	LP 0
	SBM
	LDM 
	LP REG_L
	ANMA						; (P) v A - (P)
	MVDM
	JRP l_ps_pset03
l_ps_pset02:

	LP REG_L
	ORMA						; (P) v A - (P)
	MVDM	
l_ps_pset03:	
#endasm	
}


byte v_ps_line_xp;
byte v_ps_line_yp;
byte v_ps_line_dx;
byte v_ps_line_dy;
byte v_ps_line_incrmX;
byte v_ps_line_incrmY;
word v_ps_line_dp;
word v_ps_line_NE;
word v_ps_line_SE;
/*! 
 \brief Draw a line
 
 \fn ps_line 
 \param v_ps_line_xa 
 \param v_ps_line_ya 
 \param v_ps_line_xb 
 \param v_ps_line_yb 
*/
ps_line(byte v_ps_line_xa, byte v_ps_line_ya, byte v_ps_line_xb, byte v_ps_line_yb) {
	/* pour tracer une droite a partir de 2 points : (xa,ya) et (xb,yb) */
	

	
	if (v_ps_line_xb > v_ps_line_xa) {
		v_ps_line_incrmX = 1;
		v_ps_line_dx = v_ps_line_xb - v_ps_line_xa;
	}
	else {
		v_ps_line_incrmX = 2;
		v_ps_line_dx = v_ps_line_xa - v_ps_line_xb;
	}
	
	if (v_ps_line_yb > v_ps_line_ya) {
		v_ps_line_incrmY = 1;
		v_ps_line_dy = v_ps_line_yb - v_ps_line_ya;
	}
	else {
		v_ps_line_incrmY = 2;
		v_ps_line_dy = v_ps_line_ya - v_ps_line_yb;
	}
	
	
	if (v_ps_line_dx > v_ps_line_dy) { 
		
		v_ps_line_dp =  1024 + v_ps_line_dy + v_ps_line_dy - v_ps_line_dx ;
		v_ps_line_SE =  v_ps_line_dy + v_ps_line_dy;
		v_ps_line_NE = v_ps_line_dx + v_ps_line_dx - v_ps_line_dy - v_ps_line_dy;	
		v_ps_line_yp = v_ps_line_ya;
		for( v_ps_line_xp = v_ps_line_xa ; v_ps_line_xp != v_ps_line_xb; )
		{
			ps_pset(v_ps_line_xp,v_ps_line_yp,GSET);
			
			if (v_ps_line_dp <=1024 ) { v_ps_line_dp += v_ps_line_SE;	}
			else {
				v_ps_line_dp -= v_ps_line_NE;
				if (v_ps_line_incrmY==1) {v_ps_line_yp++;}
				else {v_ps_line_yp--;}
			}
			if (v_ps_line_incrmX==1) {v_ps_line_xp++;}
			else {v_ps_line_xp--;}
			
		}
	}
	
	if (v_ps_line_dx < v_ps_line_dy)  {
		
		v_ps_line_dp = 1024 + v_ps_line_dx + v_ps_line_dx - v_ps_line_dy ;
		v_ps_line_SE = v_ps_line_dx + v_ps_line_dx;
		v_ps_line_NE = v_ps_line_dy + v_ps_line_dy - v_ps_line_dx - v_ps_line_dx;
		
		v_ps_line_xp = v_ps_line_xa;
		for( v_ps_line_yp = v_ps_line_ya ; v_ps_line_yp != v_ps_line_yb ;  ) {
			ps_pset(v_ps_line_xp,v_ps_line_yp,GSET);
			
			if (v_ps_line_dp <= 1024)  { v_ps_line_dp += v_ps_line_SE;}
			else {
				v_ps_line_dp -= v_ps_line_NE;
				if (v_ps_line_incrmX==1) {v_ps_line_xp++;}
				else {v_ps_line_xp--;}
			}
			if (v_ps_line_incrmY==1) {v_ps_line_yp++;}
			else {v_ps_line_yp--;}
		}
	}
	
} // Brensenham 

#endif // __GRAPH_H
