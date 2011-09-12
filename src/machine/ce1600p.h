#ifndef CE1600P_H
#define CE1600P_H


#include "Log.h"
#include "lh5810.h"
#include "ce150.h"
#include "cprinter.h"
#include "Connect.h"
#include "Inter.h"
#include "Keyb.h"


#define NO_MOVE	0
#define RI_MOVE	1
#define LE_MOVE 2
#define PEN_UP	0
#define PEN_DOWN 1

class Cprinter;
class Cmotor;

extern void DoCe150Dialog(void);
extern TransMap KeyMapce1600p[];
extern int KeyMapce1600pLenght;


class Cce1600p:public Cce150{

public:
    bool    init(void);
    bool	run(void);
    void    Print(void);

     Cmotor		Motor_Z;

    Cce1600p(CPObject *parent = 0);

};


#endif // CE1600P_H
