#ifndef PC1280_H
#define PC1280_H


#include "pc1360.h"


#include "Connect.h"
#include "Keyb.h"


class Cpc1280:public Cpc1360{						//PC1350 emulator main class
Q_OBJECT

    Q_PROPERTY(int angle READ angle WRITE setAngle)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom)
public:
    const char*	GetClassName(){ return("Cpc1280");}

    bool Chk_Adr(DWORD *d,DWORD data);
    bool Chk_Adr_R(DWORD *d, DWORD *data);

    virtual bool	init(void);
    virtual bool	Set_Connector(void);
    virtual bool	Get_Connector(void);
    virtual void	initExtension(void);
    virtual void TurnON(void);
    void paintEvent(QPaintEvent *event);
    virtual void TurnCLOSE(void);
    void setAngle(int value);
    int angle() const { return m_angle; }
    int m_angle;

    void setZoom(qreal value);
    qreal zoom() const { return m_zoom; }
    qreal m_zoom;

    bool flipping;
    QImage *back;
    Cpc1280(CPObject *parent = 0);

    virtual ~Cpc1280();

private:

public slots:
    void endAnimation(void);
};


#endif // PC1280_H
