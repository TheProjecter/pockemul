#ifndef _CE152_H_
#define _CE152_H_

#include "pobject.h"
#include "Connect.h"
#include "Inter.h"
#include "Keyb.h"


#define BIT_0           8
#define BIT_1           16
#define BIT_MID         ((BIT_0 + BIT_1) / 2)


#define ERR_OK	0
#define ERR_NOK	-1


typedef unsigned char    u_char;    //  8 bits
typedef unsigned short   u_short;   // 16 bits
typedef unsigned long    u_long;    // 32 bits

enum TAPEmode {PLAY,STOP,LOAD,EJECT,RECORD};

class Cce152:public CPObject{

Q_OBJECT

public:
	typedef struct {
    FILE*   ptrFd ;
    u_long  ident ;
    u_long  freq ;			// frequency
    u_long  bitLen ;
    u_long  count ;
    u_long  sum ;
    u_long  type ;
    u_long  graph ;
    u_long  debug ;
} WavFileInfo ;

typedef struct {
    FILE*   ptrFd ;
    u_long  ident ;
    u_long  count ;
    u_long  sum ;
} FileInfo ;

	BYTE	Tapein;					//Tape loaded ? (0:no,other:yes)

    virtual bool	init(void);				//initialize
    virtual bool	exit(void);				//end
    virtual bool	run(void);
	bool	GetWav(void);
	bool	SetWav(bool);



    TAPEmode mode;
	bool	paused;
	
	int		getTapeCounter(void);

	bool	SoundOn;

    WavFileInfo	info ;

	QString		TapeFileName;
	Cconnector	*pTAPECONNECTOR;
	qint64		pTAPECONNECTOR_value;
	virtual void ComputeKey(void);
	
    Cce152(CPObject *parent = 0);
    ~Cce152() {
        StopPlay();
    }

protected slots:
	int		Play(void);
	int		StopPlay(void);
	int		LoadTape(void);
	int		EjectTape(void);
	int		RecTape(void);

protected:
	int		myfgetc(WavFileInfo* ptrFile);
	long	TapeCounter;
	int		carac;
	char	*fname_wav;
	int		med_pt;
	int		low_pt;
	qint64	first_state;
	qint64	counter;
	int		GetWav_Val;

	int		UpdateHeadToWav (u_long    nbSamp,WavFileInfo* ptrFile);

	int		WriteHeadToWav (u_long    nbSamp,u_long    freq,WavFileInfo* ptrFile);
	int		WriteShortToFile (u_long value,FILE** ptrFd);
	int		WriteLongToFile (u_long value,FILE** ptrFd);
	int		WriteStringToFile (char*  ptrStr,FILE** ptrFd);
	int		myfseek(WavFileInfo* ptrFile , long offset , int origin );

	int		ReadWavHead (WavFileInfo* ptrFile);
	int		ReadStringFromFile (char*  ptrStr,long   nb,FILE** ptrFd);
	int		ReadLongFromFile (u_long* ptrVal,FILE**  ptrFd);
	int		ReadShortFromFile (u_short* ptrVal,FILE**   ptrFd);

	void	CvStringIToShort (char*    ptrStr,u_short* ptrVal);
	void	CvStringIToLong (char*   ptrStr,u_long* ptrVal);
	void	CvShortToStringI (u_short value,char*   ptrStr);
    void	CvLongToStringI (u_long value,unsigned char*  ptrStr);

    qint64  previous_state_setwav;
};

class Cce127r:public Cce152{
Q_OBJECT

public:
    Cce127r(CPObject *parent = 0);
};

class Cpc2081:public Cce152{
Q_OBJECT

public:
    Cpc2081(CPObject *parent = 0);
};

class Cce152_PC15XX:public Cce152{

public:

	int		GetBit(void);
	BYTE	getbyte(void);

	int		SetBit(bool);
	bool	getbit(void);
	int		getperiod();

	int		RecTape(void);

	int		ReadQuaterFromWav (u_long*   ptrQuat, WavFileInfo* ptrFile);
	int		ReadByteFromWav (u_long    order, u_long*   ptrByte, WavFileInfo* ptrFile);
	int		ReadBitFieldFromWav (u_long nbBits, char* ptrBits, WavFileInfo* ptrFile);
	int		SkipBitsFromWav (u_long*   ptrNb, WavFileInfo* ptrFile);
	int		SyncBitFromWav (int sign, u_long*   ptrTrans, u_long* ptrLimit, WavFileInfo* ptrFile);
	int		ReadBitFromWav (u_long*   ptrTrans,WavFileInfo* ptrFile);
	int		FindSyncFromWav (u_long freq,u_long* ptrNb,WavFileInfo*  ptrFile);
	int		FindFreqAndSyncFromWav (u_long freq,WavFileInfo*  ptrFile);
	int		serial(int bit);


	int		WriteBitToWav (u_long    value,WavFileInfo* ptrFile);
	int		WriteSyncToWav (int       nbSync,WavFileInfo* ptrFile);


	void	AddToWavBuffer(char c);
	char	WavBuffer[2048];

	
	Cce152_PC15XX(CPObject *parent = 0)	: Cce152(parent)
	{
    }




};

#endif
