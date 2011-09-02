#ifndef __GRAPH_H
#define __GRAPH_H

ps_pset(byte v_ps_pset_x, byte v_ps_pset_y) {
	if (v_ps_pset_x >= __DISP_GR_COLS__) return;
	if (v_ps_pset_y >= __DISP_GR_ROWS__) return;
	_reg_m = v_ps_pset_x;
	_reg_n = v_ps_pset_y / 8;
	_reg_k = v_ps_pset_y - (_reg_n * 8);
	
	_LCC_DEPEND(ps_dispadr);	
	
	#asm
		LP REG_N
		LDM
		EXAB
		LP REG_M
		LDM
		
		CALL fct_ps_dispadr		; Input A (graph. cur. x), B (graph. cur. y)
		; Return value: Y
		DY
		IY
		LDD							; current (DP) -> A
		LP REG_L
		EXAM						; current display column in L
		LP REG_K
		LDM							; mod8 in K
		;DECA
		PUSH
		LIA 1
		SR
		l_ps_pset01:
		SL
		LOOP l_ps_pset01			; A contient new value to OR	; 
		LP REG_L
		ORMA						; (P) v A - (P)
		EXWD
		
	#endasm
	
}

#endif // __GRAPH_H
