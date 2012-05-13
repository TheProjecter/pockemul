#ifndef MD100_H
#define MD100_H


#include <QTime>
#include <QSound>
#include <QDir>

#include "common.h"
#include "pobject.h"
#include "casiodisk.h"

#define SEC_COUNT   16      //	{ number of sectors on the track }
#define SEC_BASE    1       //	{ number of the first sector on the track }
#define BUFSIZE     1024    //	{ at least 2 * SIZE_RECORD + 4 }
#define SIZE_RECORD     256


class Cmd100:public CPObject{
    Q_OBJECT
public:

    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    virtual	bool run(void);					//check access

    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);

    virtual bool UpdateFinalImage(void);


    Cconnector	*pCONNECTOR;		qint64 pCONNECTOR_value;





    Cmd100(CPObject *parent = 0);

    virtual ~Cmd100();

public:


    BYTE SwitchCmd(BYTE x);
    BYTE ExecDir(BYTE x);
    BYTE ReturnCountHi(BYTE x);
    BYTE ReturnCountLo(BYTE x);
    BYTE ReturnBlock(BYTE x);
    BYTE ExecCloseFile(BYTE x);
    BYTE AcceptCountLo(BYTE x);
    BYTE AcceptCountHi(BYTE x);
    BYTE AcceptBlock(BYTE x);
    BYTE ExecOpenFile(BYTE x);
    BYTE ExecReadFile(BYTE x);
    BYTE ExecReadSector(BYTE x);
    BYTE ExecKillFile(BYTE x);
    BYTE ExecRenameFile(BYTE x);
    BYTE ExecWriteSector(BYTE x);
    BYTE ExecWriteFile(BYTE x);
    BYTE ExecGetSize(BYTE x);

    void FddOpen();
    void FddClose();
    BYTE CnvStatus(CcasioDOS::TDosStatusCode x);

    BYTE FddTransfer(BYTE DataIn);
protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void definePath(void);

private:
    QDir    directory;
    typedef BYTE (Cmd100::* funcPtr)(BYTE);
    static const funcPtr cmdtab[55];

    int count;
    int index;
    int bufindex;
    BYTE buffer[BUFSIZE];
    int opstatus;

    int mdFileNotOpened;
    int mdNoRoom;
    int mdInvalidCommand;
    int mdFileFound;
    int mdRenameFailed;
    int mdNoData;
    int mdWriteProtected;
    int mdEndOfFile;
    int mdOK;

    bool isdisk;
    int deindex;	// directory entry index }
    BYTE cmdcode;

    CcasioDOS fdd;

    BYTE port,data;
    bool sendData;

    bool prev_P0;
    bool prev_P1;
    bool prev_P2;
    bool prev_P3;
    bool prev_P4;
};



#endif // MD100_H
