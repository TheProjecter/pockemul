#ifndef CT6834_H
#define CT6834_H

#include <Qt>
#include "pobject.h"
#include "cx07.h"

typedef struct
 {
  qint8 lng_send;
  qint8 lng_rsp;
  char *Str_Cmd;
 } CMD_T6834;

class CT6834 : public CPObject
{
public:
    CT6834(CPObject *parent = 0);

    static const qint8 Date[8];

    qint8 Send_Cmd_T6834 [255];
    qint8 Locate_OnOff;
    qint8 Loc_X;
    qint8 Loc_Y;

    static const CMD_T6834 Cmd_T6834[0x48];
    int InitReponseT6834(qint8 Ordre, qint8 *Rsp, PorT_FX *Port);
    qint8 R5;
    Cx07 *pPC;
    void AffUdkON(qint8 State);
};

#endif // CT6834_H
