#ifndef __GRAPH_H
#define __GRAPH_H

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
	
	if (v_ps_pset_x >= __DISP_GR_COLS__) return;
	if (v_ps_pset_y >= __DISP_GR_ROWS__) return;
	_reg_m = v_ps_pset_x;
	_reg_n = v_ps_pset_y / 8;
	_reg_k = v_ps_pset_y - (_reg_n * 8);
	_reg_j = v_ps_pset_flag;
	
	_LCC_DEPEND(ps_dispadr);	
	
#asm
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



#endif // __GRAPH_H
