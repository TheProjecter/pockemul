#ifndef _POBJECT_H_
#define _POBJECT_H_

//#include <QObject>

#include <QWidget>
#include <QPoint>
#include <QImage>
#include <QMenu>
#include <QAudioOutput>

#include "common.h"

class CpcXXXX;
class Ckeyb;
class Ctimer;
class Clcdc;
class Cconnector;
class CExtensionArray;
class DialogKeyList;
class DialogDump;

class CPObject:public QWidget
{

    Q_OBJECT

public:

	CPObject(CPObject *parent=0);
	
	virtual ~CPObject();
	
	QPoint		SnapPts;
	qreal		RangeFrom(CPObject * target);
    virtual void ComputeKey(void){}
	bool		stackBehind;		// if true then always stacked behind the connected object
	CPObject	*Parent;
	CpcXXXX		*pPC;
	Ckeyb		*pKEYB;
	Ctimer		*pTIMER;
	Clcdc		*pLCDC;					// create LCDC object
	QString	BackGroundFname;
	QString	LcdFname;
	QString	SymbFname;
	
    void setName(QString val){ Name = val;	}
    QString getName(){ return Name;	}
	
    QString getcfgfname() { return cfgfname; }
    void	setcfgfname(QString s) { cfgfname = s; }
	
	int	posx();
	int posy();
	void setPosX(int);
	void setPosY(int);
	void Move(QPoint);
	QPoint pos();
	QImage* FinalImage;
	QImage* BackgroundImageBackup;
	QImage* BackgroundImage;
	QImage* LcdImage;
	QImage* SymbImage;

	int		Pc_DX,Pc_DY;			
	int		Pc_DX_mm,Pc_DY_mm,Pc_DZ_mm;		// PC size (mm)	

	QRect rect();
	virtual	bool	init();			// initialize
	virtual bool	InitDisplay(void);
	virtual void	UpdateFinalImage(void);
    virtual	bool	run(void){ return true;}					// emulator main step
	virtual bool	exit();					// exit
    int		getfrequency() { return frequency; }
    void	setfrequency(int f) { frequency = f; }

	QImage * LoadImage(QSize size,QString fname);
	bool	Refresh_Display;

	QList<Cconnector*> ConnList;
	void publish(Cconnector* newConn);

	void BuildContextMenu(QMenu * menu);
	void computeLinkMenu(QMenu * menu);
	void computeUnLinkMenu(QMenu * menu);
	
	CExtensionArray *extensionArray[5];
	bool	toDestroy;

	TransMap *KeyMap;
	int		KeyMapLenght;
	BYTE	LastKey;
    int     zoom;
	

	virtual int initsound();
	virtual int exitsound();
	void fillSoundBuffer(BYTE val);
#ifndef NO_SOUND
	FSOUND_STREAM* pStream;
	static signed char F_CALLBACKAPI CustomStreamCallBack( FSOUND_STREAM *stream, void *buff, int len, void * param );
#endif
    QList<unsigned char> soundBuffer;
	
    QAudioDeviceInfo* m_device;
    QAudioFormat    m_format;
    QAudioOutput*   m_audioOutput;
    QIODevice*      m_output;
    int DataFrequencyHz;
    int BufferSize;

	DialogKeyList	*dialogkeylist;
    DialogDump		*dialogdump;

	void setCpu(int );
    virtual void TurnON() {}
    virtual void TurnOFF() {}
	bool	Power;
    virtual void Reset() {}
    void    SwitchFrontBack(QPoint);
    bool    isFront() {return Front;}
		
public:

	void paintEvent(QPaintEvent *);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mouseDoubleClickEvent ( QMouseEvent * event );
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent( QMouseEvent * event );
	void focusInEvent ( QFocusEvent * event ); 
	void focusOutEvent ( QFocusEvent * event ); 
	
private slots:
    void audioStateChanged(QAudio::State state);
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

	
private:
	int		PosX,PosY;
	int		Width,Height;

	bool	startKeyDrag;
	QPoint	KeyDrag;
	bool	startPosDrag;
	QPoint	PosDrag;
	int		frequency;

	QString Name;
	QString cfgfname;				// configration file name
	
	bool	Front;
	int		iChanIndex;
	qint64	fillSoundBuffer_old_state;	
};


#endif
