#ifndef _DASM_H
#define _DASM_H
void DoDasmDialog();
void RefreshDasm();

extern int g_DasmStep;
extern int g_DasmFlag;
extern DWORD g_BreakPointAdr;
extern int g_BreakSubLevel;

#endif
