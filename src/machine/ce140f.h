#ifndef CE140F_H
#define CE140F_H

/********************************************************************************************************
 * PROGRAM      : test
 * DATE - TIME  : samedi 28 octobre 2006 - 12h40
 * AUTHOR       :  (  )
 * FILENAME     : Ce126.h
 * LICENSE      : GPL
 * COMMENTARY   : printer(CE-126P) emulation class
 ********************************************************************************************************/

#include <QTime>
#include <QSound>


#include "cprinter.h"
#include "Keyb.h"
#include "Connect.h"
#include "Inter.h"



class Cce140f:public CPObject{
public:

    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    virtual	bool run(void);					//check access

    virtual void clearPaper(void);
    virtual void SaveAsText(void);

    virtual void ComputeKey(void);

    //virtual void resizeEvent ( QResizeEvent * );

    Cconnector	*pCONNECTOR;		qint64 pCONNECTOR_value;
    Cconnector	*pCONNECTOR_Ext;	qint64 pCONNECTOR_Ext_value;

//    QImage *ce140fbuf;
//    QImage *ce140fdisplay;

    void RefreshCe140f(qint8 data);
    QSound *bells;

    bool	ToDestroy;

    void	settop(int value){ top = value; }
    void	setposX(int value) { posX = value; }
int ce140f_Mode;

    void Push8(BYTE b);
    void Push4(BYTE b);
    BYTE Pop_out8(void);
    BYTE Pop_out4(void);

    void processCommand(void);

    void process_DSKF(void);
    void process_FILES(void);
    void process_INIT(int cmd);
    void process_FILES_LIST(void);
    void process_SAVE(int cmd);
    void process_LOAD(int cmd);
    void process_CLOSE(int cmd);
    void process_KILL(int cmd);

    void sendString(QString s);
    BYTE CheckSum(BYTE b);

    Cce140f(CPObject *parent = 0);

    ~Cce140f()
    {
    }

public:
    bool    rmtSwitch;

private:
    void	Printer(qint8 d);			//printer emulator
    QImage *charTable;
    void	pulldownsignal(void);

    int		top;
    int		posX;
    QByteArray	TextBuffer;
    bool	Previous_PIN_BUSY;
    bool	Previous_PIN_MT_OUT1;
    bool	Previous_PIN_D_OUT;
   // QTime	time;
    bool	ctrl_char;
    BYTE	t,c;
    qint64	run_oldstate;
    int     code_transfer_step;
    int     device_code;
    QList<BYTE>   data;
    QList<BYTE> data_out;
    bool    halfdata;
    bool    halfdata_out;
    int     checksum;
    int     wait_data_function;
    qint64     file_size;

    QFile   file_save;
    QFile   file_load;
    QByteArray ba_load;
    qint64  lastState;
};


#endif // CE140F_H
