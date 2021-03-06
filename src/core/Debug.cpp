/*** PC-1350 Emulator [DEBUG.CC] **************************/
/* debugger                                               */
/**********************************************************/
#include <stdlib.h>
#include <stdio.h>


#include "common.h"
#include "Debug.h"
#include "pcxxxx.h"
#include "cpu.h"

//#include "inter.h"

DisAsmTbl	AsmTbl_sc61860[]={
{		 2,"LII   %02X",	"LII"},					/* 00h */
{		 2,"LIJ   %02X",	"LIJ"},
{		 2,"LIA   %02X",	"LIA"},
{		 2,"LIB   %02X",	"LIB"},
{		 1,"IX",			"IX"},
{		 1,"DX",			"DX"},
{		 1,"IY",			"IY"},
{		 1,"DY",			"DY"},
{		 1,"MVW",			"MVW"},
{		 1,"EXW",			"EXW"},
{		 1,"MVB",			"MVB"},
{		 1,"EXB",			"EXB"},
{		 1,"ADN",			"ADN"},
{		 1,"SBN",			"SBN"},
{		 1,"ADW",			"ADW"},
{		 1,"SBW",			"SBW"},
{		 3,"LIDP  %02X%02X","LIDP"},					/* 10h */
{		 2,"LIDL  %02X",	"LIDL"},
{		 2,"LIP   %02X",	"LIP"},
{		 2,"LIQ   %02X",	"LIQ"},
{		 1,"ADB",			"ADB"},
{		 1,"SBB",			"SBB"},
{		 1,"???   (16)",	"???   (16)"},
{		 1,"???   (17)",	"???   (17)"},
{		 1,"MVWD",			"MVWD"},
{		 1,"EXWD",			"EXWD"},
{		 1,"MVBD",			"MVBD"},
{		 1,"EXBD",			"EXBD"},
{		 1,"SRW",			"SRW"},
{		 1,"SLW",			"SLW"},
{		 1,"FILM",			"FILM"},
{		 1,"FILD",			"FILD"},
{		 1,"LDP",			"LDP"},							/* 20h */
{		 1,"LDQ",			"LDQ"},
{		 1,"LDR",			"LDR"},
{         1,"CLRA",          "CLRA"},
{		 1,"IXL",			"IXL"},
{		 1,"DXL",			"DXL"},
{		 1,"IYS",			"IYS"},
{		 1,"DYS",			"DYS"},
{		 2,"JRNZP %02X",	"JRNZP"},
{		 2,"JRNZM %02X",	"JRNZM"},
{		 2,"JRNCP %02X",	"JRNCP"},
{		 2,"JRNCM %02X",	"JRNCM"},
{		 2,"JRP   %02X",	"JRP"},
{		 2,"JRM   %02X",	"JRM"},
{		 1,"???   (2E)",	"???   (2E)"},
{		 2,"LOOP  %02X",	"LOOP"},
{		 1,"STP",			"STP"},							// 30h
{		 1,"STQ",			"STQ"},
{		 1,"STR",			"STR"},
{		 1,"???   (33)",	"???   (33)"},
{		 1,"PUSH",			"PUSH"},
{		 1,"DATA",			"DATA"},
{		 1,"???	  (36)",	"???   (36)"},
{		 1,"RTN",			"RTN"},
{		 2,"JRZP  %02X",	"JRZP"},
{		 2,"JRZM  %02X",	"JRZM"},
{		 2,"JRCP  %02X",	"JRCP"},
{		 2,"JRCM  %02X",	"JRCM"},
{		 1,"???   (3C)",	"???   (3C)"},
{		 1,"???   (3D)",	"???   (3D)"},
{		 1,"???   (3E)",	"???   (3E)"},
{		 1,"???   (3F)",	"???   (3F)"},
{		 1,"INCI",			"INCI"},							/* 40h */
{		 1,"DECI",			"DECI"},
{		 1,"INCA",			"INCA"},
{		 1,"DECA",			"DECA"},
{		 1,"ADM",			"ADM"},
{		 1,"SBM",			"SBM"},
{		 1,"ANMA",			"ANMA"},
{		 1,"ORMA",			"ORMA"},
{		 1,"INCK",			"INCK"},
{		 1,"DECK",			"DECK"},
{		 1,"INCM",			"INCM"},
{		 1,"DECM",			"DECM"},
{         1,"IN A",			"IN A"},
{		 1,"NOPW",			"NOPW"},
{		 2,"WAIT  %02X",	"WAIT"},
{		 1,"CUP",			"CUP"},
{		 1,"INCP",			"INCP"},							/* 50h */
{		 1,"DECP",			"DECP"},
{		 1,"STD",			"STD"},
{		 1,"MVDM",			"MVDM"},
{		 1,"READM",			"READM"},
{		 1,"MVMD",			"MVMD"},
{		 1,"READ",			"READ"},
{		 1,"LDD",			"LDD"},
{		 1,"SWP",			"SWP"},
{		 1,"LDM",			"LDM"},
{		 1,"SL",			"SL"},
{		 1,"POP",			"POP"},
{		 1,"???   (5C)",	"???   (5C)"},
{         1,"OUT A",			"OUT A"},
{		 1,"???   (5E)",	"???   (5E)"},
{		 1,"OUT F",			"OUT F"},
{		 2,"ANIM  %02X",	"ANIM"},						// 60h
{		 2,"ORIM  %02X",	"ORIM"},
{		 2,"TSIM  %02X",	"TSIM"},
{		 2,"CPIM  %02X",	"CPIM"},
{		 2,"ANIA  %02X",	"ANIA"},
{		 2,"ORIA  %02X",	"ORIA"},
{		 2,"TSIA  %02X",	"TSIA"},
{		 2,"CPIA  %02X",	"CPIA"},
{		 1,"???   (68)",	"???   (68)"},
{		-3,"CASE 2",		"CASE 2"},
{		 1,"???   (6A)",	"???   (6A)"},
{		 2,"TEST  %02X",	"TEST"},
{		 1,"???   (6C)",	"???   (6C)"},
{		 1,"???   (6D)",	"???   (6D)"},
{		 1,"???   (6E)",	"???   (6E)"},
{		 1,"CDN",			"CDN"},
{		 2,"ADIM  %02X",	"ADIM"},						// 70h
{		 2,"SBIM  %02X",	"SBIM"},
{		 1,"???   (72)",	"???   (72)"},
{		 1,"???   (73)",	"???   (73)"},
{		 2,"ADIA  %02X",	"ADIA"},
{		 2,"SBIA  %02X",	"SBIA"},
{		 1,"???   (76)",	"???   (76)"},
{		 1,"???   (77)",	"???   (77)"},
{		 3,"CALL  %02X%02X","CALL"},
{		 3,"JP    %02X%02X","JP"},
{        -4,"CASE1 %02X-(%02X%02X)","CASE1"},
{		 1,"???   (7B)",	"???   (7B)"},
{		 3,"JPNZ  %02X%02X","JPNZ"},
{		 3,"JPNC  %02X%02X","JPNC"},
{		 3,"JPZ   %02X%02X","JPZ"},
{		 3,"JPC   %02X%02X","JPC"},
{		-1,"LP%02X",		"LP"},							// 80h
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},

{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",		"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},

{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},

{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},
{		-1,"LP%02X",	"LP"},

{		 1,"INCJ",		"INCJ"},						// C0h
{		 1,"DECJ","DECJ"},
{		 1,"INCB","INCB"},
{		 1,"DECB","DECB"},
{		 1,"ADCM","ADCM"},
{		 1,"SBCM","SBCM"},
{		 1,"??? ","??? "},
{		 1,"CPMA","CPMA"},
{		 1,"INCL","INCL"},
{		 1,"DECL","DECL"},
{		 1,"INCN","INCN"},
{		 1,"DECN","DECN"},
{         1,"IN B ","IN B "},
{		 1,"??? ","??? "},
{		 1,"NOPT","NOPT"},
{		 1,"??? ","??? "},

{		 1,"SC  ","SC  "},
{		 1,"RC  ","RC  "},
{		 1,"SR  ","SR  "},
{		 1,"??? ","??? "},
{		 2,"ANID  %02X","ANID"},
{		 2,"ORID  %02X","ORID"},
{		 2,"TSID  %02X","TSID"},
{		 1,"???   (D7)","???   (D7)"},
{		 1,"LEAVE","LEAVE"},
{		 1,"???   (D9)","???   (D9)"},
{		 1,"EXAB","EXAB"},
{		 1,"EXAM","EXAM"},
{		 1,"???   (DC)","???   (DC)"},
{         1,"OUT B","OUT B"},
{		 1,"???   (DE)","???   (DE)"},
{         1,"OUT C","OUT C"},

{	    -2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},

{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"},
{		-2,"CAL%02X%02X","CAL"}
};

DisAsmTbl	AsmTbl_lh5801[]={
    {		 2,"LII   %02X",	"LII"},					/* 00h */
    {		 2,"LIJ   %02X",	"LIJ"},
    {		 2,"LIA   %02X",	"LIA"},
    {		 2,"LIB   %02X",	"LIB"},
    {		 1,"IX",			"IX"},
    {		 1,"DX",			"DX"},
    {		 1,"IY",			"IY"},
    {		 1,"DY",			"DY"},
    {		 1,"MVW",			"MVW"},
    {		 1,"EXW",			"EXW"},
    {		 1,"MVB",			"MVB"},
    {		 1,"EXB",			"EXB"},
    {		 1,"ADN",			"ADN"},
    {		 1,"SBN",			"SBN"},
    {		 1,"ADW",			"ADW"},
    {		 1,"SBW",			"SBW"},
    {		 3,"LIDP  %02X%02X","LIDP"},					/* 10h */
    {		 2,"LIDL  %02X",	"LIDL"},
    {		 2,"LIP   %02X",	"LIP"},
    {		 2,"LIQ   %02X",	"LIQ"},
    {		 1,"ADB",			"ADB"},
    {		 1,"SBB",			"SBB"},
    {		 1,"???   (16)",	"???   (16)"},
    {		 1,"???   (17)",	"???   (17)"},
    {		 1,"MVWD",			"MVWD"},
    {		 1,"EXWD",			"EXWD"},
    {		 1,"MVBD",			"MVBD"},
    {		 1,"EXBD",			"EXBD"},
    {		 1,"SRW",			"SRW"},
    {		 1,"SLW",			"SLW"},
    {		 1,"FILM",			"FILM"},
    {		 1,"FILD",			"FILD"},
    {		 1,"LDP",			"LDP"},							/* 20h */
    {		 1,"LDQ",			"LDQ"},
    {		 1,"LDR",			"LDR"},
    {         1,"CLRA",          "CLRA"},
    {		 1,"IXL",			"IXL"},
    {		 1,"DXL",			"DXL"},
    {		 1,"IYS",			"IYS"},
    {		 1,"DYS",			"DYS"},
    {		 2,"JRNZP %02X",	"JRNZP"},
    {		 2,"JRNZM %02X",	"JRNZM"},
    {		 2,"JRNCP %02X",	"JRNCP"},
    {		 2,"JRNCM %02X",	"JRNCM"},
    {		 2,"JRP   %02X",	"JRP"},
    {		 2,"JRM   %02X",	"JRM"},
    {		 1,"???   (2E)",	"???   (2E)"},
    {		 2,"LOOP  %02X",	"LOOP"},
    {		 1,"STP",			"STP"},							// 30h
    {		 1,"STQ",			"STQ"},
    {		 1,"STR",			"STR"},
    {		 1,"???   (33)",	"???   (33)"},
    {		 1,"PUSH",			"PUSH"},
    {		 1,"DATA",			"DATA"},
    {		 1,"???	  (36)",	"???   (36)"},
    {		 1,"RTN",			"RTN"},
    {		 2,"JRZP  %02X",	"JRZP"},
    {		 2,"JRZM  %02X",	"JRZM"},
    {		 2,"JRCP  %02X",	"JRCP"},
    {		 2,"JRCM  %02X",	"JRCM"},
    {		 1,"???   (3C)",	"???   (3C)"},
    {		 1,"???   (3D)",	"???   (3D)"},
    {		 1,"???   (3E)",	"???   (3E)"},
    {		 1,"???   (3F)",	"???   (3F)"},
    {		 1,"INCI",			"INCI"},							/* 40h */
    {		 1,"DECI",			"DECI"},
    {		 1,"INCA",			"INCA"},
    {		 1,"DECA",			"DECA"},
    {		 1,"ADM",			"ADM"},
    {		 1,"SBM",			"SBM"},
    {		 1,"ANMA",			"ANMA"},
    {		 1,"ORMA",			"ORMA"},
    {		 1,"INCK",			"INCK"},
    {		 1,"DECK",			"DECK"},
    {		 1,"INCM",			"INCM"},
    {		 1,"DECM",			"DECM"},
    {         1,"IN A",			"IN A"},
    {		 1,"NOPW",			"NOPW"},
    {		 2,"WAIT  %02X",	"WAIT"},
    {		 1,"CUP",			"CUP"},
    {		 1,"INCP",			"INCP"},							/* 50h */
    {		 1,"DECP",			"DECP"},
    {		 1,"STD",			"STD"},
    {		 1,"MVDM",			"MVDM"},
    {		 1,"READM",			"READM"},
    {		 1,"MVMD",			"MVMD"},
    {		 1,"READ",			"READ"},
    {		 1,"LDD",			"LDD"},
    {		 1,"SWP",			"SWP"},
    {		 1,"LDM",			"LDM"},
    {		 1,"SL",			"SL"},
    {		 1,"POP",			"POP"},
    {		 1,"???   (5C)",	"???   (5C)"},
    {         1,"OUT A",			"OUT A"},
    {		 1,"???   (5E)",	"???   (5E)"},
    {		 1,"OUT F",			"OUT F"},
    {		 2,"ANIM  %02X",	"ANIM"},						// 60h
    {		 2,"ORIM  %02X",	"ORIM"},
    {		 2,"TSIM  %02X",	"TSIM"},
    {		 2,"CPIM  %02X",	"CPIM"},
    {		 2,"ANIA  %02X",	"ANIA"},
    {		 2,"ORIA  %02X",	"ORIA"},
    {		 2,"TSIA  %02X",	"TSIA"},
    {		 2,"CPIA  %02X",	"CPIA"},
    {		 1,"???   (68)",	"???   (68)"},
    {		-3,"CASE 2",		"CASE 2"},
    {		 1,"???   (6A)",	"???   (6A)"},
    {		 2,"TEST  %02X",	"TEST"},
    {		 1,"???   (6C)",	"???   (6C)"},
    {		 1,"???   (6D)",	"???   (6D)"},
    {		 1,"???   (6E)",	"???   (6E)"},
    {		 1,"CDN",			"CDN"},
    {		 2,"ADIM  %02X",	"ADIM"},						// 70h
    {		 2,"SBIM  %02X",	"SBIM"},
    {		 1,"???   (72)",	"???   (72)"},
    {		 1,"???   (73)",	"???   (73)"},
    {		 2,"ADIA  %02X",	"ADIA"},
    {		 2,"SBIA  %02X",	"SBIA"},
    {		 1,"???   (76)",	"???   (76)"},
    {		 1,"???   (77)",	"???   (77)"},
    {		 3,"CALL  %02X%02X","CALL"},
    {		 3,"JP    %02X%02X","JP"},
    {        -4,"CASE1 %02X-(%02X%02X)","CASE1"},
    {		 1,"???   (7B)",	"???   (7B)"},
    {		 3,"JPNZ  %02X%02X","JPNZ"},
    {		 3,"JPNC  %02X%02X","JPNC"},
    {		 3,"JPZ   %02X%02X","JPZ"},
    {		 3,"JPC   %02X%02X","JPC"},
    {		-1,"LP%02X",		"LP"},							// 80h
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},

    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",		"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},

    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},

    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},
    {		-1,"LP%02X",	"LP"},

    {		 1,"INCJ",		"INCJ"},						// C0h
    {		 1,"DECJ","DECJ"},
    {		 1,"INCB","INCB"},
    {		 1,"DECB","DECB"},
    {		 1,"ADCM","ADCM"},
    {		 1,"SBCM","SBCM"},
    {		 1,"??? ","??? "},
    {		 1,"CPMA","CPMA"},
    {		 1,"INCL","INCL"},
    {		 1,"DECL","DECL"},
    {		 1,"INCN","INCN"},
    {		 1,"DECN","DECN"},
    {         1,"IN B ","IN B "},
    {		 1,"??? ","??? "},
    {		 1,"NOPT","NOPT"},
    {		 1,"??? ","??? "},

    {		 1,"SC  ","SC  "},
    {		 1,"RC  ","RC  "},
    {		 1,"SR  ","SR  "},
    {		 1,"??? ","??? "},
    {		 2,"ANID  %02X","ANID"},
    {		 2,"ORID  %02X","ORID"},
    {		 2,"TSID  %02X","TSID"},
    {		 1,"???   (D7)","???   (D7)"},
    {		 1,"LEAVE","LEAVE"},
    {		 1,"???   (D9)","???   (D9)"},
    {		 1,"EXAB","EXAB"},
    {		 1,"EXAM","EXAM"},
    {		 1,"???   (DC)","???   (DC)"},
    {         1,"OUT B","OUT B"},
    {		 1,"???   (DE)","???   (DE)"},
    {         1,"OUT C","OUT C"},

    {	    -2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},

    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"},
    {		-2,"CAL%02X%02X","CAL"}
    };

DWORD Cdebug::DisAsm_File(DWORD adr,FILE *fp)
{
	DWORD Result;

	Result = DisAsm_1(adr);
	fprintf(fp,"%s",Buffer);
	return(Result);
}


void Cdebug::header(DWORD adr,int l) {
    sprintf(Buffer,"%05X:",(uint) adr);
    for(int i=0;i<l;i++)
        sprintf(Buffer,"%s%02X",Buffer,(uint)pPC->pCPU->get_mem(adr+i,SIZE_8));
}

/*****************************************************************************/
/* Initializeing Debugger													 */
/*  ENTRY :none																 */
/*  RETURN:none																 */
/*****************************************************************************/
bool Cdebug::init(void)
{
	return true;
}

/*****************************************************************************/
/* Exitting Debugger														 */
/*  ENTRY :none																 */
/*  RETURN:none																 */
/*****************************************************************************/
bool Cdebug::exit(void)
{
	return true;
}

DWORD Cdebug_sc61860::DisAsm_1(DWORD adr)
{

    //const char	*reg[]={"A","IL","BA","I","X","Y","U","S"};
	char	l;
//	char	b[16];
	char	s[2000];
//	BYTE	t;
	BYTE	i;

	Buffer[0] = '\0';
	DasmAdr = adr;
    if(adr==20076) {
        adr=20076;
    }
    int a = pPC->pCPU->get_mem(adr,SIZE_8);
    l=abs(AsmTbl[a].len);
    switch(AsmTbl[pPC->Get_PC(adr)].len)
	{
	case  1:sprintf(s,AsmTbl[pPC->pCPU->get_mem(adr,SIZE_8)].nim);
			break;
	case  2:sprintf(s,AsmTbl[pPC->pCPU->get_mem(adr,SIZE_8)].nim,pPC->pCPU->get_mem(adr+1,SIZE_8));
			break;
	case  3:sprintf(s,AsmTbl[pPC->pCPU->get_mem(adr,SIZE_8)].nim,pPC->pCPU->get_mem(adr+1,SIZE_8),pPC->pCPU->get_mem(adr+2,SIZE_8));
			break;
	case  4:sprintf(s,AsmTbl[pPC->pCPU->get_mem(adr,SIZE_8)].nim,pPC->pCPU->get_mem(adr+1,SIZE_8),pPC->pCPU->get_mem(adr+2,SIZE_8),pPC->pCPU->get_mem(adr+3,SIZE_8));
			break;

	case -1:sprintf(s,AsmTbl[pPC->pCPU->get_mem(adr,SIZE_8)].nim,(pPC->pCPU->get_mem(adr,SIZE_8)&0x3F));
			break;
	case -2:sprintf(s,AsmTbl[pPC->pCPU->get_mem(adr,SIZE_8)].nim,(pPC->pCPU->get_mem(adr,SIZE_8)&0x1F),pPC->pCPU->get_mem(adr+1,SIZE_8));
			break;
	case -4:sprintf(s,AsmTbl[pPC->pCPU->get_mem(adr,SIZE_8)].nim,pPC->pCPU->get_mem(adr+1,SIZE_8),pPC->pCPU->get_mem(adr+2,SIZE_8),pPC->pCPU->get_mem(adr+3,SIZE_8));
	
            sprintf(Buffer,"%05X:",(uint)adr);
			for(i=0;i<l;i++)
                sprintf(Buffer,"%s%02X",Buffer,(uint)pPC->pCPU->get_mem(adr+i,SIZE_8));
			sprintf(Buffer,"%s%*s%s\n",Buffer,16-(l<<1)," ",s);

			{
			int pc,nb,rh,rl;

			pc = adr;

			pc++; nb = pPC->pCPU->get_mem(pc,SIZE_8);
			pc++; rh = pPC->pCPU->get_mem(pc,SIZE_8);
			pc++; rl = pPC->pCPU->get_mem(pc,SIZE_8);
			pc++; // CASE 2
			sprintf(s,"CASE 2 ");
			
			pc++;
			for (int i=0;i<nb;i++)
			{
				sprintf(s,"%s,%02X:%02X%02X",
					s,
                    (uint)pPC->pCPU->get_mem(pc,SIZE_8),
                    (uint)pPC->pCPU->get_mem(pc+1,SIZE_8),
                    (uint)pPC->pCPU->get_mem(pc+2,SIZE_8));
				pc+=3;
			}
			adr +=4;
			l = 5+3*nb;
			}		
			break;

	
	}

	
    sprintf(Buffer,"%s%05X:",Buffer,(uint)adr);
	if (l<5)
		for(i=0;i<l;i++)
            sprintf(Buffer,"%s%02X",Buffer,(uint)pPC->pCPU->get_mem(adr+i,SIZE_8));
	int decal;
	if (l<5) decal = l;
	else	decal = 0;
	sprintf(Buffer,"%s%*s%s",Buffer,16-(decal<<1)," ",s);


	debugged = 1;
	NextDasmAdr = ((adr+l)&MASK_20);

	return((adr+l)&MASK_20);
}

////////////////////////////////////////////////////////////
typedef enum { 
	Imp,
	Reg,
	Vec, // imm byte (vector at 0xffxx)
	Vej,
	Imm,
	RegImm,
	Imm16,
	RegImm16,
	ME0,
	ME0Imm,
	Abs,
	AbsImm,
	ME1,
	ME1Imm,
	ME1Abs,
	ME1AbsImm,
	RelP,
	RelM
} Adr;

typedef enum {
	RegNone,
	A,
	XL, XH, X,
	YL, YH, Y,
	UL, UH, U,
	P, S
} Regs;

const char *RegNames[]= {
	0, "A", "XL", "XH", "X", "YL", "YH", "Y", "UL", "UH", "U", "P", "S"
};

typedef enum {
	ILL, ILL2, PREFD, NOP,

	LDA, STA, LDI, LDX, STX,
	LDE, SDE, LIN, SIN,
	TIN, // (x++)->(y++)
	ADC, ADI, ADR, SBC, SBI, 
	DCA, DCS, // bcd add and sub
	CPA, CPI, CIN, // A compared with (x++)
	AND, ANI, ORA, ORI, EOR, EAI, BIT, BII,
	INC, DEC,
	DRL, DRR, // digit rotates
	ROL, ROR,
	SHL, SHR,
	AEX, // A nibble swap

	BCR, BCS, BHR, BHS, BZR, BZS, BVR, BVS, 
	BCH, LOP, // loop with ul
	JMP, SJP, RTN, RTI, HLT,
	VCR, VCS, VHR, VHS, VVS, VZR, VZS,
	VMJ, VEJ,
	PSH, POP, ATT, TTA,
	REC, SEC, RIE, SIE,

	AM0, AM1, // load timer reg
	ITA, // reads input port
	ATP, // akku send to data bus
	CDV, // clears internal divider
	OFF, // clears bf flip flop
	RDP, SDP,// reset display flip flop
	RPU, SPU,// flip flop pu off
	RPV, SPV,// flip flop pv off
} Ins;

const char *InsNames[]={
	"ILL", "ILL", 0, "NOP",
	"LDA", "STA", "LDI", "LDX", "STX",
	"LDE", "SDE", "LIN", "SIN",
	"TIN",
	"ADC", "ADI", "ADR", "SBC", "SBI", 
	"DCA", "DCS", 
	"CPA", "CPI", "CIN",
	"AND", "ANI", "ORA", "ORI", "EOR", "EAI", "BIT", "BII",
	"INC", "DEC", 
	"DRL", "DRR",
	"ROL", "ROR",
	"SHL", "SHR",
	"AEX", 
	"BCR", "BCS", "BHR", "BHS", "BZR", "BZS", "BVR", "BVS", 
	"BCH", "LOP", 
	"JMP", "SJP", "RTN", "RTI", "HLT",
	"VCR", "VCS", "VHR", "VHS", "VVS", "VZR", "VZS",
	"VMJ", "VEJ",
	"PSH", "POP", "ATT", "TTA",
	"REC", "SEC", "RIE", "SIE",

	"AM0", "AM1",
	"ITA",
	"ATP",
	"CDV",
	"OFF",
	"RDP", "SDP",
	"RPU", "SPU",
	"RPV", "SPV",
};

typedef struct { Ins ins; Adr adr; Regs reg; } Entry;

static const Entry table[0x100]={
	{ SBC, Reg, XL }, // 0
	{ SBC, ME0, X },
	{ ADC, Reg, XL },
	{ ADC, ME0, X },
	{ LDA, Reg, XL },
	{ LDA, ME0, X },
	{ CPA, Reg, XL },
	{ CPA, ME0, X },
	{ STA, Reg, XH },
	{ AND, ME0, X },
	{ STA, Reg, XL },
	{ ORA, ME0, X },
	{ DCS, ME0, X },
	{ EOR, ME0, X },
	{ STA, ME0, X },
	{ BIT, ME0, X },
	{ SBC, Reg, YL }, // 0x10
	{ SBC, ME0, Y },
	{ ADC, Reg, YL },
	{ ADC, ME0, Y },
	{ LDA, Reg, YL },
	{ LDA, ME0, Y },
	{ CPA, Reg, YL },
	{ CPA, ME0, Y },
	{ STA, Reg, YH },
	{ AND, ME0, Y },
	{ STA, Reg, YL },
	{ ORA, ME0, Y },
	{ DCS, ME0, Y },
	{ EOR, ME0, Y },
	{ STA, ME0, Y },
	{ BIT, ME0, Y },
	{ SBC, Reg, UL }, // 0x20
	{ SBC, ME0, U },
	{ ADC, Reg, UL },
	{ ADC, ME0, U },
	{ LDA, Reg, UL },
	{ LDA, ME0, U },
	{ CPA, Reg, UL },
	{ CPA, ME0, U },
	{ STA, Reg, UH },
	{ AND, ME0, U },
	{ STA, Reg, UL },
	{ ORA, ME0, U },
	{ DCS, ME0, U },
	{ EOR, ME0, U },
	{ STA, ME0, U },
	{ BIT, ME0, U },
	{ ILL }, // 0x30
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ NOP, Imp },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ INC, Reg, XL }, //0x40
	{ SIN, Reg, X },
	{ DEC, Reg, XL },
	{ SDE, Reg, X },
	{ INC, Reg, X },
	{ LIN, Reg, X },
	{ DEC, Reg, X },
	{ LDE, Reg, X },
	{ LDI, RegImm, XH },
	{ ANI, ME0Imm, X },
	{ LDI, RegImm, XL },
	{ ORI, ME0Imm, X },
	{ CPI, RegImm, XH },
	{ BII, ME0Imm, X },
	{ CPI, RegImm, XL },
	{ ADI, ME0Imm, X },
	{ INC, Reg, YL }, //0x50
	{ SIN, Reg, Y },
	{ DEC, Reg, YL },
	{ SDE, Reg, Y },
	{ INC, Reg, Y },
	{ LIN, Reg, Y },
	{ DEC, Reg, Y },
	{ LDE, Reg, Y },
	{ LDI, RegImm, YH },
	{ ANI, ME0Imm, Y },
	{ LDI, RegImm, YL },
	{ ORI, ME0Imm, Y },
	{ CPI, RegImm, YH },
	{ BII, ME0Imm, Y },
	{ CPI, RegImm, YL },
	{ ADI, ME0Imm, Y },
	{ INC, Reg, UL }, //0x60
	{ SIN, Reg, U },
	{ DEC, Reg, UL },
	{ SDE, Reg, U },
	{ INC, Reg, U },
	{ LIN, Reg, U },
	{ DEC, Reg, U },
	{ LDE, Reg, U },
	{ LDI, RegImm, UH },
	{ ANI, ME0Imm, U },
	{ LDI, RegImm, UL },
	{ ORI, ME0Imm, U },
	{ CPI, RegImm, UH },
	{ BII, ME0Imm, U },
	{ CPI, RegImm, UL },
	{ ADI, ME0Imm, U },
	{ ILL }, // 0X70
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ ILL },
	{ SBC, Reg, XH }, // 0x80
	{ BCR, RelP },
	{ ADC, Reg, XH },
	{ BCS, RelP },
	{ LDA, Reg, XH },
	{ BHR, RelP },
	{ CPA, Reg, XH },
	{ BHS, RelP },
	{ LOP, RelM },
	{ BZR, RelP },
	{ RTI, Imp },
	{ BZS, RelP },
	{ DCA, ME0, X },
	{ BVR, RelP },
	{ BCH, RelP },
	{ BVS, RelP },
	{ SBC, Reg, YH }, // 0x90
	{ BCR, RelM },
	{ ADC, Reg, YH },
	{ BCS, RelM },
	{ LDA, Reg, YH },
	{ BHR, RelM },
	{ CPA, Reg, YH },
	{ BHS, RelM },
	{ ILL },
	{ BZR, RelM },
	{ RTN, Imp },
	{ BZS, RelM },
	{ DCA, ME0, Y },
	{ BVR, RelM },
	{ BCH, RelM },
	{ BVS, RelM },
	{ SBC, Reg, UH }, // 0xa0
	{ SBC, Abs },
	{ ADC, Reg, UH },
	{ ADC, Abs },
	{ LDA, Reg, UH },
	{ LDA, Abs },
	{ CPA, Reg, UH },
	{ CPA, Abs },
	{ SPV, Imp },
	{ AND, Abs },
	{ LDI, RegImm16, S },
	{ ORA, Abs },
	{ DCA, ME0, U },
	{ EOR, Abs },
	{ STA, Abs },
	{ BIT, Abs },
	{ ILL }, //0xb0
	{ SBI },
	{ ILL },
	{ ADI, RegImm, A },
	{ ILL },
	{ LDI, RegImm, A },
	{ ILL },
	{ CPI, RegImm, A },
	{ RPV, Imp },
	{ ANI, RegImm, A },
	{ JMP, Imm16 },
	{ ORI, RegImm, A },
	{ ILL },
	{ EAI, Imm },
	{ SJP, Imm16 },
	{ BII, RegImm, A },
	{ VEJ, Vej }, // 0xc0
	{ VCR, Vec },
	{ VEJ, Vej },
	{ VCS, Vec },
	{ VEJ, Vej },
	{ VHR, Vec },
	{ VEJ, Vej },
	{ VHS, Vec },
	{ VEJ, Vej },
	{ VZR, Vec },
	{ VEJ, Vej },
	{ VZS, Vec },
	{ VEJ, Vej },
	{ VMJ, Vec },
	{ VEJ, Vej },
	{ VVS, Vec },
	{ VEJ, Vej }, // 0xd0
	{ ROR, Imp },
	{ VEJ, Vej },
	{ DRR, Imp },
	{ VEJ, Vej },
	{ SHR, Imp },
	{ VEJ, Vej },
	{ DRL, Imp },
	{ VEJ, Vej },
	{ SHL, Imp },
	{ VEJ, Vej },
	{ ROL, Imp },
	{ VEJ, Vej },
	{ INC, Reg, A },
	{ VEJ, Vej },
	{ DEC, Reg, A },
	{ VEJ, Vej }, //0xe0
	{ SPU, Imp },
	{ VEJ, Vej },
	{ RPU, Imp },
	{ VEJ, Vej },
	{ ILL },
	{ VEJ, Vej },
	{ ILL },
	{ VEJ, Vej },
	{ ANI, AbsImm },
	{ VEJ, Vej },
	{ ORI, AbsImm },
	{ VEJ, Vej },
	{ BII, AbsImm },
	{ VEJ, Vej },
	{ ADI, AbsImm },
	{ VEJ, Vej }, //0xf0
	{ AEX, Imp },
	{ VEJ, Vej },
	{ ILL },
	{ VEJ, Vej },
	{ TIN, Imp },
	{ VEJ, Vej },
	{ CIN, Imp },
	{ ILL },
	{ REC, Imp },
	{ ILL },
	{ SEC, Imp },
	{ ILL },
	{ PREFD },
	{ ILL },
	{ ILL }
};
static const Entry table_fd[0x100]={
	{ ILL2 }, // 0x00
	{ SBC, ME1, X },
	{ ILL2 },
	{ ADC, ME1, X },
	{ ILL2 },
	{ LDA, ME1, X },
	{ ILL2 },
	{ CPA, ME1, X },
	{ LDX, Reg, X },
	{ AND, ME1, X },
	{ POP, Reg, X },
	{ ORA, ME1, X },
	{ DCS, ME1, X },
	{ EOR, ME1, X },
	{ STA, ME1, X },
	{ BIT, ME1, X },
	{ ILL2 }, // 0x10
	{ SBC, ME1, Y },
	{ ILL2 },
	{ ADC, ME1, Y },
	{ ILL2 },
	{ LDA, ME1, Y },
	{ ILL2 },
	{ CPA, ME1, Y },
	{ LDX, Reg, Y },
	{ AND, ME1, Y },
	{ POP, Reg, Y },
	{ ORA, ME1, Y },
	{ DCS, ME1, Y },
	{ EOR, ME1, Y },
	{ STA, ME1, Y },
	{ BIT, ME1, Y },
	{ ILL2 }, // 0x20
	{ SBC, ME1, U },
	{ ILL2 },
	{ ADC, ME1, U },
	{ ILL2 },
	{ LDA, ME1, U },
	{ ILL2 },
	{ CPA, ME1, U },
	{ LDX, Reg, U },
	{ AND, ME1, U },
	{ POP, Reg, U },
	{ ORA, ME1, U },
	{ DCS, ME1, U },
	{ EOR, ME1, U },
	{ STA, ME1, U },
	{ BIT, ME1, U },
	{ ILL2 }, // 0x30
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ INC, Reg, XH }, // 0x40
	{ ILL2 },
	{ DEC, Reg, XH }, //46 in other part of manual
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ LDX, Reg, S },
	{ ANI, ME1Imm, X },
	{ STX, Reg, X },
	{ ORI, ME1Imm, X },
	{ OFF, Imp },
	{ BII, ME1Imm, X },
	{ STX, Reg, S },
	{ ADI, ME1Imm, X },
	{ INC, Reg, YH }, // 0x50
	{ ILL2 },
	{ DEC, Reg, YH }, // 56 in other part of manual
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ LDX, Reg, P },
	{ ANI, ME1Imm, Y },
	{ STX, Reg, Y },
	{ ORI, ME1Imm, Y },
	{ ILL2 },
	{ BII, ME1Imm, Y },
	{ STX, Reg, P },
	{ ADI, ME1Imm, Y },
	{ INC, Reg, UH }, // 0x60
	{ ILL2 },
	{ DEC, Reg, UH }, // 66 in other part of manual
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ANI, ME1Imm, U },
	{ STX, Reg, U },
	{ ORI, ME1Imm, U },
	{ ILL2 },
	{ BII, ME1Imm, U },
	{ ILL  },
	{ ADI, ME1Imm, U },
	{ ILL2 }, // 0x70
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 }, // 0x80
	{ SIE, Imp },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ PSH, Reg, X },
	{ ILL2 },
	{ POP, Reg, A },
	{ ILL2 },
	{ DCA, ME1, X },
	{ ILL2 },
	{ CDV, Imp },
	{ ILL2 },
	{ ILL2 }, // 0x90
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ PSH, Reg, Y },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ DCA, ME1, Y },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 }, // 0xa0
	{ SBC, ME1Abs },
	{ ILL2 },
	{ ADC, ME1Abs },
	{ ILL2 },
	{ LDA, ME1Abs },
	{ ILL2 },
	{ CPA, ME1Abs },
	{ PSH, Reg, U },
	{ AND, ME1Abs },
	{ TTA, Imp },
	{ ORA, ME1Abs },
	{ DCA, ME1, U },
	{ EOR, ME1Abs },
	{ STA, ME1Abs },
	{ BIT, ME1Abs },
	{ ILL2 }, // 0xb0
	{ HLT, Imp },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ITA, Imp },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ RIE, Imp },
	{ ILL2 },
	{ RDP, Imp }, // 0xc0
	{ SDP, Imp },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ PSH, Reg, A },
	{ ILL2 },
	{ ADR, Reg, X },
	{ ILL2 },
	{ ATP, Imp },
	{ ILL2 },
	{ AM0, Imp },
	{ ILL2 },
	{ ILL2 }, // 0xd0
	{ ILL2 },
	{ ILL2 },
	{ DRR, ME1, X },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ DRL, ME1, X },
	{ ILL2 },
	{ ILL2 },
	{ ADR, Reg, Y },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ AM1, Imp },
	{ ILL2 },
	{ ILL2 }, // 0xe0
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ANI, ME1AbsImm },
	{ ADR, Reg, U },
	{ ORI, ME1AbsImm },
	{ ATT, Imp },
	{ BII, ME1AbsImm },
	{ ILL2 },
	{ ADI, ME1AbsImm },
	{ ILL2 }, // 0xf0	
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 },
	{ ILL2 }
};


DWORD Cdebug_lh5801::DisAsm_1(DWORD oldpc)
{
	int pc;
	int oper;
	UINT16 absolut;
	const Entry *entry;
	int temp;
	char LocBuffer[60];

	oldpc&=0xffff;
	DasmAdr = oldpc;
	pc=oldpc;
	oper=pPC->Get_8(pc++);
	entry=table+oper;

	if (table[oper].ins==PREFD) {
		oper=pPC->Get_8(pc++);
		entry=table_fd+oper;
	}
	switch (entry->ins) {
	case ILL:
		sprintf(LocBuffer,"%s %.2x", InsNames[entry->ins], oper);break;
	case ILL2:
		sprintf(LocBuffer,"%s fd%.2x", InsNames[entry->ins], oper);break;
	default:
		switch(entry->adr) {			
		case Imp:
			sprintf(LocBuffer,"%s", InsNames[entry->ins]);break;
		case Reg:
			sprintf(LocBuffer,"%s %s", InsNames[entry->ins],RegNames[entry->reg]);break;
		case RegImm:
			sprintf(LocBuffer,"%s %s,%.2x", InsNames[entry->ins],
					RegNames[entry->reg], pPC->Get_8(pc++));
			break;
		case RegImm16:
			absolut=pPC->Get_8(pc++)<<8;
			absolut|=pPC->Get_8(pc++);
			sprintf(LocBuffer,"%s %s,%.4x", InsNames[entry->ins],RegNames[entry->reg],absolut );
			break;
		case Vec:
			sprintf(LocBuffer,"%s (ff%.2x)", InsNames[entry->ins],pPC->Get_8(pc++));break;
		case Vej:
			sprintf(LocBuffer,"%s (ff%.2x)", InsNames[entry->ins], oper);break;		
		case Imm:
			sprintf(LocBuffer,"%s %.2x", InsNames[entry->ins],pPC->Get_8(pc++));break;
		case Imm16:
			absolut=pPC->Get_8(pc++)<<8;
			absolut|=pPC->Get_8(pc++);
			sprintf(LocBuffer,"%s %.4x", InsNames[entry->ins],absolut );break;
		case RelP:
			temp=pPC->Get_8(pc++);
			sprintf(LocBuffer,"%s %.4x", InsNames[entry->ins],pc+temp );break;
		case RelM:
			temp=pPC->Get_8(pc++);
			sprintf(LocBuffer,"%s %.4x", InsNames[entry->ins],pc-temp );break;
		case Abs:
			absolut=pPC->Get_8(pc++)<<8;
			absolut|=pPC->Get_8(pc++);
			sprintf(LocBuffer,"%s (%.4x)", InsNames[entry->ins],absolut );break;
		case ME1Abs:
			absolut=pPC->Get_8(pc++)<<8;
			absolut|=pPC->Get_8(pc++);
			sprintf(LocBuffer,"%s #(%.4x)", InsNames[entry->ins],absolut );break;
		case AbsImm:
			absolut=pPC->Get_8(pc++)<<8;
			absolut|=pPC->Get_8(pc++);
			sprintf(LocBuffer,"%s (%.4x),%.2x", InsNames[entry->ins],absolut, 
					pPC->Get_8(pc++));break;
		case ME1AbsImm:
			absolut=pPC->Get_8(pc++)<<8;
			absolut|=pPC->Get_8(pc++);
			sprintf(LocBuffer,"%s #(%.4x),%.2x", InsNames[entry->ins],absolut, 
					pPC->Get_8(pc++));break;
		case ME0:
			sprintf(LocBuffer,"%s (%s)", InsNames[entry->ins],RegNames[entry->reg] );break;		
		case ME0Imm:
			sprintf(LocBuffer,"%s (%s),%.2x", InsNames[entry->ins],RegNames[entry->reg],pPC->Get_8(pc++) );
			break;		
		case ME1:
			sprintf(LocBuffer,"%s #(%s)", InsNames[entry->ins],RegNames[entry->reg] );break;
		case ME1Imm:
			sprintf(LocBuffer,"%s #(%s),%.2x", InsNames[entry->ins],RegNames[entry->reg],pPC->Get_8(pc++) );
			break;		
		}		
	}
	debugged = 1;
	NextDasmAdr = pc;
    sprintf(Buffer,"%05X: %s",(uint)oldpc,LocBuffer);

	return pc;

}

