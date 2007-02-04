#include "common.h"

int g_DasmFlag = 0;
int g_DasmStep = 0;

DWORD	g_BreakPointAdr = 9999999; //-- MAXDWORD;
int		g_BreakSubLevel = -1;

//static int		Index		= 0;
//static DWORD	MaxAdr		= 0;
//static DWORD	NextMaxAdr	= 0;

bool IsAdrInList(DWORD adr)
{
#if 0
	int NbLig;

	NbLig = ListBox_GetCount(g_hWndListDasm);

#if 1
	for (int i=0;i<NbLig;i++)
	{
		if (adr == ListBox_GetItemData(g_hWndListDasm, i))
		{
			ListBox_SetCurSel(g_hWndListDasm, i);
			return(TRUE);
		}
	}
#else
	int i=ListBox_FindItemData(g_hWndListDasm,0,adr);
	if (i>=0)
	{
		ListBox_SetCurSel(g_hWndListDasm,i );
		return(TRUE);
	}
#endif
#endif
	return(FALSE);

}



void RefreshDasm()
{
#if 0
	if (g_hWndListDasm && g_DasmFlag && pPC->pCPU->pDEBUG->debugged)
	{
		char	text[100];

		if (! IsAdrInList(pPC->pCPU->pDEBUG->DasmAdr))
		{
			if (MaxAdr > pPC->pCPU->pDEBUG->DasmAdr)
			{
				// effacer tout et recommencer au debut
				ListBox_ResetContent(g_hWndListDasm);
				Index		= 0;
				MaxAdr		= pPC->pCPU->pDEBUG->DasmAdr;
				NextMaxAdr	= pPC->pCPU->pDEBUG->NextDasmAdr;
			}
			else
			{
				if (pPC->pCPU->pDEBUG->DasmAdr > NextMaxAdr)
				{
					// Insert a separator
					sprintf(text,"%05X:---------------------",pPC->pCPU->pDEBUG->DasmAdr-1);
					ListBox_AddItemData(g_hWndListDasm,text);
					ListBox_SetItemData(g_hWndListDasm, Index, 0); 
					NextMaxAdr = pPC->pCPU->pDEBUG->NextDasmAdr;
					Index++;
				}
				MaxAdr		= pPC->pCPU->pDEBUG->DasmAdr;
				NextMaxAdr	= pPC->pCPU->pDEBUG->NextDasmAdr;
			}

			ListBox_AddItemData(g_hWndListDasm,pPC->pCPU->pDEBUG->Buffer);
			ListBox_SetItemData(g_hWndListDasm, Index, pPC->pCPU->pDEBUG->DasmAdr); 
			
			ListBox_SetCurSel(g_hWndListDasm, Index);
			Index++;
// full until 15 lines
			for (int j=Index;j<15;j++)
			{
				pPC->pCPU->pDEBUG->DisAsm_1(NextMaxAdr);
				MaxAdr		= pPC->pCPU->pDEBUG->DasmAdr;
				NextMaxAdr	= pPC->pCPU->pDEBUG->NextDasmAdr;
				ListBox_AddItemData(g_hWndListDasm,pPC->pCPU->pDEBUG->Buffer);
				ListBox_SetItemData(g_hWndListDasm, Index, pPC->pCPU->pDEBUG->DasmAdr); 
				Index++;
			}
		}
		ShowReg();
	}
#endif
}


#if 0
static bool CALLBACK DasmProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char	Buffer[10];

		switch (msg)
		{
		case WM_COMMAND:
			switch (LOWORD (wparam))
			{
				case IDOK:
				case IDCANCEL:
				{
					g_hWndDlgDasm = 0;
					::EndDialog (hwnd, LOWORD (wparam));
					break;
				}
				
				case IDCLEAR:
				{
					ListBox_ResetContent(g_hWndListDasm);
					break;
				}
				case IDSTART:
				{
					g_DasmFlag = 0;
					pPC->pCPU->halt = 0;
					Edit_GetText(g_hWndBreakDasm,Buffer,5);
					sscanf(Buffer,"%04x",&g_BreakPointAdr);
					ListBox_ResetContent(g_hWndListDasm);
					Index = 0;
					if (g_BreakSubLevel == pPC->pCPU->CallSubLevel)
						g_BreakSubLevel = -1;

					break;
				}
				case IDSTEPOVER:
				{
#if 0
					g_BreakSubLevel = pPC->pCPU->CallSubLevel;
					g_DasmFlag = 0;
					pPC->pCPU->halt = 0;
					Edit_GetText(g_hWndBreakDasm,Buffer,5);
					sscanf(Buffer,"%04x",&g_BreakPointAdr);
					ListBox_ResetContent(g_hWndListDasm);
					Index = 0;
#else
					g_DasmFlag = 0;
					pPC->pCPU->halt = 0;
					g_BreakPointAdr = pPC->pCPU->get_PC();
					ListBox_ResetContent(g_hWndListDasm);
					Index = 0;
#endif
					break;
				}
				case IDSTOP:
				{
					g_DasmFlag = 0;
					pPC->pCPU->halt = 1;
					break;
				}
				case IDSTEP:
				{
					g_DasmFlag = 1;
					pPC->pCPU->halt = 0;
					g_DasmStep = 1;
					break;
				}

			}
			break;

		case WM_INITDIALOG:
		{
			::CenterWindow (hwnd);

			break;
		}

		default:
			return FALSE;
	}

	return TRUE;
}
#endif

void DoDasmDialog ()
{
#if 0
//	static HWND hWndDlgDasm;

	if (!g_hWndDlgDasm)
	{
		g_hWndDlgDasm = CreateDialog(gInstance, MAKEINTRESOURCE (IDD_DASM), hwnd, &::DasmProc);
		ShowWindow(g_hWndDlgDasm, SW_SHOWNORMAL);

		g_hWndListDasm = ::GetDlgItem (g_hWndDlgDasm, IDC_LISTDASM);
		g_hWndBreakDasm= ::GetDlgItem (g_hWndDlgDasm, IDC_BREAKVAL);

		HFONT hfnt =CreateFont(
					15,		// height of font
					0,		// average character width
					0,		// angle of escapement
					0,		// base-line orientation angle
					FW_BOLD,		// font weight
					0,		// italic attribute option
					0,		// underline attribute option
					0,		// strikeout attribute option
					0,		// character set identifier
					0,		// output precision
					0,		// clipping precision
					0,		// output quality
			FIXED_PITCH ,	// pitch and family
			"terminal"		// typeface name
);

			SendMessage( 
				(HWND) g_hWndListDasm,    // handle to destination window 
				WM_SETFONT,               // message to send
				(WPARAM) hfnt,          // handle to font
				(LPARAM) TRUE           // redraw option
						);



	}
#endif
}
