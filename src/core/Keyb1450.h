#ifndef KEYB1450_H
#define KEYB1450_H

//TODO  ADD {} in KeyMap1450 to avoid warning during compilation

BYTE scandef_pc1450[] = {
//+0		+1			+2			+3			+4			+5			+6			+7 
'7',		'8',		'9',		'/',		K_XM,		NUL,		NUL,		NUL,
'4',		'5',		'6',		'*',		K_RM,		K_SHT,		K_DEF,		K_SML,
'1',		'2',		'3',		'-',		K_MPLUS,	'q',		'a',		'z',
'0',		K_SIGN,		'.',		'+',		'=',		'w',		's',		'x',
K_HYP,		K_SIN,		K_COS,		K_TAN,		NUL/*Kanji*/,'e',		'd',		'c',
K_HEX,		K_DEG,		K_LN,		K_LOG,		'[',		'r',		'f',		'v',
K_EXP,		K_POT,		K_ROOT,		K_SQR,		'\\',		't',		'g',		'b',
NUL,		NUL,		NUL,		NUL,		NUL,		NUL,		NUL,		NUL,
NUL,		K_CCE,		K_STAT,		K_FSE,		K_DA,		'y',		'h',		'n',
NUL,		NUL,		')',		K_1X,		K_UA,		'u',		'j',		'm',
NUL,		NUL,		NUL,		'(',		K_LA,		'i',		'k',		' ',
NUL,		NUL,		NUL,		NUL,		K_RA,		'o',		'l',		K_RET,
NUL,		NUL,		NUL,		NUL,		NUL,		'p',		',',		K_BASIC,
NUL,		NUL,		NUL,		NUL,		NUL,		NUL,		'Z',		K_CAL,
NUL,		NUL,		NUL,		NUL,		NUL,		NUL,		NUL,		K_OF
};

TransMap KeyMap1450[]={
1,	"  -         ",	'-',	584,	181,	2,		//OK
2,	"  (         ",	'(',	552,	77,		1,		//OK
3,	"  )         ",	')',	589,	77,		1,		//OK
4,	"  *         ",	'*',	584,	144,	2,		//OK
5,	"  ,         ",	',',	401,	189,	1,		//OK
6,	"  .         ",	'.',	540,	217,	2,		//OK
7,	"  /         ",	'/',	584,	107,	2,		//OK
8,	"  :         ",	':',	0,		0,		1,		//OK
9,	"  ;         ",	';',	0,		0,		1,		//OK
10,	"  +         ",	'+',	584,	217,	2,		//OK
11,	" CLS        ",	K_CCE,	635,	22,		1,		//OK
12,	" DEF        ",	K_DEF,	25,		189,	1,		//OK
13,	" DEL        ",	K_DEL,	0,		0,		1,
14,	" Down Arrow ",	K_DA,	250,	126,	1,		//OK	
15,	" ENTER      ",	K_RET,	363,	221,	3,		//OK
16,	" Equal      ",	'=',	629,	217,	2,		//OK
17,	" INS        ",	K_INS,	0,		0,		1,
18,	" Left Arrow ",	K_LA ,	326,	126,	1,		//OK
19,	" MODE       ",	K_MOD,	0,		0,		1,
20,	" On/Brk     ",	K_BRK,	100,	126,	1,		//OK	
21,	" On/Off     ",	K_OF ,	25,		55,		4,		//OK
22,	" Rigth Arrow",	K_RA ,	363,	126,	1,		//OK	
23,	" SHIFT1     ",	K_SHT,	401,	126,	1,		//OK	
24,	" SHIFT2     ",	K_SHT,	25,		157,		1,
25,	" SML        ",	K_SML,	25,		221,		1,
26,	" Space      ",	' ',	326,	221,	1,		//OK
27,	" Up Arrow   ",	K_UA ,	288,	126,	1,		//OK	
28, "CAL         ", K_CAL,	25,		126,	1,		//OK
29,	"0           ",	'0',	449,	218,	2,		//OK
30,	"1           ",	'1',	449,	181,	2,		//OK
31,	"2           ",	'2',	494,	181,	2,		//OK
32,	"3           ",	'3',	540,	181,	2,		//OK
33,	"4           ",	'4',	449,	144,	2,		//OK
34,	"5           ",	'5',	494,	144,	2,		//OK
35,	"6           ",	'6',	540,	144,	2,		//OK
36,	"7           ",	'7',	449,	107,	2,		//OK
37,	"8           ",	'8',	494,	107,	2,		//OK
38,	"9           ",	'9',	540,	107,	2,		//OK
39,	"A           ",	'a',	63,		189,	1,		//OK
40,	"B           ",	'b',	212,	221,	1,
41,	"C           ",	'c',	137,	221,	1,
42,	"D           ",	'd',	137,	189,	1,
43,	"E           ",	'e',	137,	157,	1,
44,	"F           ",	'f',	176,	189,	1,
45,	"G           ",	'g',	212,	189,	1,
46,	"H           ",	'h',	250,	189,	1,
47,	"I           ",	'i',	326,	157,	1,
48,	"J           ",	'j',	288,	189,	1,
49,	"K           ",	'k',	326,	189,	1,
50,	"L           ",	'l',	363,	189,	1,
51,	"M           ",	'm',	288,	221,	1,
52,	"N           ",	'n',	250,	221,	1,
53,	"O           ",	'o',	363,	157,	1,
54,	"P           ",	'p',	401,	157,	1,
55,	"Q           ",	'q',	63,		157,	1,		//OK
56,	"R           ",	'r',	176,	157,	1,
57,	"S           ",	's',	100,	189,	1,
58,	"T           ",	't',	212,	157,	1,
59,	"U           ",	'u',	288,	157,	1,
60,	"V           ",	'v',	176,	221,	1,
61,	"W           ",	'w',	100,	157,	1,
62,	"X           ",	'x',	100,	221,	1,
63,	"Y           ",	'y',	250,	157,	1,
64,	"Z           ",	'z',	63,		221,	1,		//OK
65, "BASIC       ", K_BASIC,63,		126,	1,		//OK
66,	"+/-         ",	K_SIGN,	494,	217,	2,		//OK
67,	"x^2         ",	K_SQR,	561,	77,		1,		//OK
68, "SQRT(x)     ",	K_ROOT,	524,	77,		1,		//OK
69,	"x^y         ",	K_POT,	486,	77,		1,		//OK
70,	"Exp         ",	K_EXP,	449,	77,		1,		//OK
71,	"X:M         ",	K_XM,	629,	107,	2,		//OK
72,	"Stat        ",	K_STAT,	635,	50,		1,		//OK
73,	"1/x         ",	K_1X,	598,	50,		1,		//OK
74,	"Log         ",	K_LOG,	561,	50,		1,		//OK
75,	"Ln          ",	K_LN,	524,	50,		1,		//OK
76,	"Deg         ",	K_DEG,	486,	50,		1,		//OK
77,	"Hex         ",	K_HEX,	449,	50,		1,		//OK
78,	"M+          ",	K_MPLUS,629,	181,	2,		//OK
79,	"FSE         ",	K_FSE,	598,	22,		1,		//OK
80,	"Tan         ",	K_TAN,	561,	22,		1,		//OK
81,	"Cos         ",	K_COS,	524,	22,		1,		//OK
82,	"Sin         ",	K_SIN,	486,	22,		1,		//OK
83,	"Hyp         ",	K_HYP,	449,	22,		1,		//OK
84,	"RM          ",	K_RM,	629,	144,	2,		//OK
};
int KeyMap1450Lenght = 84;

#endif
