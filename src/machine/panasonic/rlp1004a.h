#ifndef RLP1004A_H
#define RLP1004A_H



#include <QTime>
#ifndef NO_SOUND
#include <QSound>
#endif

#include "common.h"
#include "cprinter.h"

#include "Inter.h"

class Cconnector;

class Crlp1004a:public Cprinter{
    Q_OBJECT
public:

    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    virtual	bool run(void);					//check access

    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);

    virtual void clearPaper(void);
    virtual void SaveAsText(void);

    virtual void ComputeKey(void);

    //virtual void resizeEvent ( QResizeEvent * );

    Cconnector	*pCONNECTOR;		qint64 pCONNECTOR_value;

    Cconnector	*pTAPECONNECTOR;	qint64 pTAPECONNECTOR_value;

    QImage *paperbuf;
    QImage *paperdisplay;

    void Refresh(quint8 data);
#ifndef NO_SOUND
    QSound *bells;
#endif

    void	settop(int value){ top = value; }
    void	setposX(int value) { posX = value; }


    Crlp1004a(CPObject *parent = 0);

    virtual ~Crlp1004a();

public:
    bool    rmtSwitch;
    void	Printer(qint8 d);			//printer emulator
    QImage *charTable;
    int margin;
    int		top;
    bool INTrequest;
protected:
    void paintEvent(QPaintEvent *);
protected slots:
    void contextMenuEvent ( QContextMenuEvent * );
    void Rotate();

private:


    bool rotate;

    int		posX;

    bool	ctrl_char;
    BYTE	t,c;
    quint64	run_oldstate;
    quint64  lastState;
    bool    tapeOutput,tapeInput;
    QByteArray buffer;
    bool printing;
    bool receiveMode;
    bool CRLFPending;

public:

};


#endif // RLP1004A_H
