#ifndef HP82143A_H
#define HP82143A_H


#include "cprinter.h"

class DialogConsole;

class Chp82143A:public Cprinter{
Q_OBJECT
public:

    virtual bool run(void);
    virtual bool init(void);				//initialize
    virtual bool exit(void);				//end
    //virtual void ExportByte(qint8);
    virtual void ComputeKey(void);
    virtual bool UpdateFinalImage(void);


    Chp82143A(CPObject *parent = 0);
    virtual ~Chp82143A();


    virtual void Printer(quint8 data);

    void printLine();

    Cconnector *pCONNECTOR;		qint64 pCONNECTOR_value;


    QImage *printerbuf;
    QImage *printerdisplay;

    void	settop(int value){ top = value; }
    void	setposX(int value) { posX = value; }


    QImage *charTable;
    int margin;
    int	top;

    int	posX;
    int paperWidth;
    int charsize;
    quint16 getStatus(void);
    void clearPaper();
protected:

protected slots:


private:


};

#endif // HP82143A_H
