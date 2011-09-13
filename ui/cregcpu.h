#ifndef CREGCPU_H
#define CREGCPU_H

#include <QWidget>

class CCPU;

class CregCPU : public QWidget
{
    Q_OBJECT
public:
    explicit CregCPU(QWidget *parent = 0,CCPU *pCPU = 0);

    virtual void refresh()=0;
    CCPU *pCPU;

signals:

public slots:

private:


};

#endif // CREGCPU_H
