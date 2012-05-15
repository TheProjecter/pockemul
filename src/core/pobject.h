#ifndef _POBJECT_H_
#define _POBJECT_H_

//#include <QObject>

#include <QWidget>
#include <QPoint>
#include <QImage>
#include <QMenu>

#ifndef NO_SOUND
#include <QAudioOutput>
#endif

#include <QXmlStreamWriter>
#include <QPixmap>

#include "common.h"

class CpcXXXX;
class Ckeyb;
class Ctimer;
class Clcdc;
class Cconnector;
class CExtensionArray;
class DialogKeyList;
class DialogDump;
class DialogDasm;
class TapAndHoldGesture;


struct URL {
    QString desc;
    QString url;
};

#define PS_OFF	0
#define PS_RUN	1
#define PS_RSV	2
#define PS_PRO	3

class CPObject:public QWidget
{

    Q_OBJECT

public:

	CPObject(CPObject *parent=0);
	
	virtual ~CPObject();
	
	QPoint		SnapPts;
	qreal		RangeFrom(CPObject * target);
    virtual void ComputeKey(void){}
	CPObject	*Parent;
	CpcXXXX		*pPC;
	Ckeyb		*pKEYB;
	Ctimer		*pTIMER;
	Clcdc		*pLCDC;					// create LCDC object
    QString BackGroundFname;
	QString	LcdFname;
	QString	SymbFname;

    int rate;

    bool	off;
    bool closed;

    void setDisp_on(bool v);
    bool getdisp_onRaised();
    bool getDisp_on(void);
    int     ioFreq;
	
    void setName(QString val){ Name = val;	}
    QString getName(){ return Name;	}
	
    QString getcfgfname() { return cfgfname; }
    void	setcfgfname(QString s) { cfgfname = s; }
	
    void    serialize(QXmlStreamWriter *,int id);
    float	posx();
    float   posy();
    void    setPosX(float);
    void    setPosY(float);
    void    Move(QPoint);
    void    MoveWithLinked(QPoint);
    QPoint   pos();
	QImage* FinalImage;
	QImage* BackgroundImageBackup;
	QImage* BackgroundImage;
	QImage* LcdImage;
	QImage* SymbImage;

    int     getDX() {return Pc_DX;}
    int     getDY() {return Pc_DY;}
    int     getDZ() {return 0;}
    void    setDX(int v) {Pc_DX = v;}
    void    setDY(int v) {Pc_DY = v;}
    void    setDZ(int v) {}

    int     getDXmm() {return Pc_DX_mm;}
    int     getDYmm() {return Pc_DY_mm;}
    int     getDZmm() {return Pc_DZ_mm;}
    void    setDXmm(int v) {Pc_DX_mm = v;}
    void    setDYmm(int v) {Pc_DY_mm = v;}
    void    setDZmm(int v) {Pc_DZ_mm = v;}


	QRect rect();
	virtual	bool	init();			// initialize
	virtual bool	InitDisplay(void);
    virtual bool	UpdateFinalImage(void);
    virtual	bool	run(void){ return true;}					// emulator main step
            qint64     runRange(qint64);
	virtual bool	exit();					// exit
    virtual bool	Set_Connector(void) { return true; }
    virtual bool	Get_Connector(void) { return true; }

    int		getfrequency() { return frequency; }
    void	setfrequency(int f) { frequency = f; }

	QImage * LoadImage(QSize size,QString fname);
	bool	Refresh_Display;

	QList<Cconnector*> ConnList;
	void publish(Cconnector* newConn);
    void remove(Cconnector* newConn);

	void BuildContextMenu(QMenu * menu);
    void computeWebLinksMenu(QMenu * menu);
    void insertLinkAction(QString desc,QString link);
    QMenu * menuweblink;
	void computeLinkMenu(QMenu * menu);
	void computeUnLinkMenu(QMenu * menu);
	
	CExtensionArray *extensionArray[5];
	bool	toDestroy;

	TransMap *KeyMap;
	int		KeyMapLenght;
	BYTE	LastKey;
    bool	Front;
	

	virtual int initsound();
	virtual int exitsound();
	void fillSoundBuffer(BYTE val);

    QList<unsigned char> soundBuffer;

#ifndef NO_SOUND
    QAudioDeviceInfo* m_device;
    QAudioFormat    m_format;
    QAudioOutput*   m_audioOutput;
#endif

    QIODevice*      m_output;
    int DataFrequencyHz;
    int BufferSize;

	DialogKeyList	*dialogkeylist;
    DialogDump		*dialogdump;
    DialogDasm      *dialogdasm;

    QPixmap         mask;

	void setCpu(int );
    virtual void TurnON() {}
    virtual void TurnOFF() {}
	bool	Power;
    qint8		PowerSwitch;
    virtual void Reset() {}
    void    SwitchFrontBack(QPoint);
    bool    isFront() {return Front;}
    QList<Cconnector *> nearConnectors(Cconnector *refConnector,qint8 snaprange);
    void manageStackPos(QList<CPObject *> *);

    virtual bool    SaveSession_File(QXmlStreamWriter *xmlOut);
    virtual bool	LoadSession_File(QXmlStreamReader *);
		
public:

    void paintEvent(QPaintEvent *);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mouseDoubleClickEvent ( QMouseEvent * event );
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent( QMouseEvent * event );
    void wheelEvent(QWheelEvent *event);
	void focusInEvent ( QFocusEvent * event ); 
	void focusOutEvent ( QFocusEvent * event ); 



    void changeGeometry(int newposx, int newposy, int newwidth, int newheight);

    virtual void raise();

    virtual void TurnCLOSE(void);

signals:
    void msgError(QString);

public slots:
#ifndef NO_SOUND
    void audioStateChanged(QAudio::State state);
#endif

    void tapAndHold(QMouseEvent *);
    void contextMenuEvent ( QContextMenuEvent * event );
	void slotExit();
	void KeyList();

    void slotCpu(QAction*);

    void slotContrast(QAction *);

	void slotPower();
	void slotReset();
    //void slotDetach();
    void slotLoadSession();
    void slotSaveSession();
	void Dump();
    void Dasm();

	
private:
    float		PosX,PosY;
	int		Width,Height;

	bool	startKeyDrag;
	QPoint	KeyDrag;
	bool	startPosDrag;
	QPoint	PosDrag;
	int		frequency;

	QString Name;
	QString cfgfname;				// configration file name
	

	int		iChanIndex;
	qint64	fillSoundBuffer_old_state;	
    QByteArray audioBuff;
    QByteArray tempBuff;

    int		Pc_DX,Pc_DY;
    int		Pc_DX_mm,Pc_DY_mm,Pc_DZ_mm;		// PC size (mm)

    bool disp_on;
    bool disp_onRaised;

    TapAndHoldGesture* _gestureHandler;

};


#endif
