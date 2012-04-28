#ifndef CPB1000_H
#define CPB1000_H

#include "pcxxxx.h"

class CHD44352;

class Cpb1000 : public CpcXXXX
{
    Q_OBJECT

    Q_PROPERTY(int angle READ angle WRITE setAngle)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom)
public:
    Cpb1000(CPObject *parent = 0);
    virtual bool init();
    virtual bool run();
    virtual bool Chk_Adr(DWORD *d, DWORD data);
    virtual bool Chk_Adr_R(DWORD *d, DWORD data);
    virtual UINT8 in(UINT8 Port);
    virtual UINT8 out(UINT8 Port, UINT8 Value);
    virtual void TurnOFF();
    virtual void TurnON();
    virtual void Reset();
    virtual bool LoadConfig(QXmlStreamReader *xmlIn);
    virtual bool SaveConfig(QXmlStreamWriter *xmlOut);

    virtual void paintEvent(QPaintEvent *event);

    CHD44352 *pHD44352;
    UINT16 getKey();
    void setKey(UINT8 data);
    UINT8 m_kb_matrix;
    WORD Get_16rPC(DWORD adr);
    UINT8 readPort(void);
    void writePort(UINT8);
    UINT8 lcdDataRead();
    void lcdDataWrite(UINT8 data);
    void lcdControlWrite(UINT8 data);


    bool flipping;
    QImage *back;


//    void keyPressEvent(QKeyEvent *event);
//    void keyReleaseEvent(QKeyEvent *event);

//    bool shift;
    //    bool fct;
    virtual void TurnCLOSE(void);
    void setAngle(int value);
    int angle() const { return m_angle; }
    int m_angle;

    void setZoom(qreal value);
    qreal zoom() const { return m_zoom; }
    qreal m_zoom;

public slots:
    void endAnimation(void);
};

#endif // CPB1000_H
