#include <QFileDialog>
#include <QTime>

#include "common.h"
#include "pcxxxx.h"
#include "Log.h"
#include "dialoganalog.h"
#include "sc61860.h"
#include "ce152.h"
#include "Keyb.h"
 
#define sample_freq		44100
#define rec_speed		1200
#define BASE_FREQ1      4000
#define BASE_FREQ2      2500
#define TRANS_HIGH      0x80
#define TRANS_LOW       0x7F
#define ORDER_STD       0
#define ORDER_INV       1
#define SYNC_NB_TRY     50
#define IDENT_UNKNOWN   0x00
#define IDENT_OLD_BAS   0x20
#define IDENT_OLD_PAS   0x21
#define IDENT_OLD_BIN   0x26
#define IDENT_NEW_BAS   0x70
#define IDENT_NEW_PAS   0x71
#define IDENT_EXT_BAS   0x72
#define IDENT_EXT_PAS   0x73
#define IDENT_NEW_BIN   0x76
#define IDENT_PC1211    0x80
#define IDENT_PC1500    0x0A
#define IDENT_PC15_BIN  0xA0
#define IDENT_PC15_BAS  0xA1

static char* bitwav[] = {
    "\255\255\0\0\255\255\0\0\255\255\0\0\255\255\0\0",
    "\255\0\255\0\255\0\255\0\255\0\255\0\255\0\255\0"} ;

char *ftyp[] = {"BASIC (prg)","BASIC (dat)","binaire","assembleur"};
FILE	*fp_tape=NULL;

//extern u_long testReadQuarterTape(void);


/*
	RIFF Chunk (12 bytes in length total) Byte Number
		0 - 3	"RIFF" (ASCII Characters)
		4 - 7	Total Length Of Package To Follow (Binary, little endian)
		8 - 12	"WAVE" (ASCII Characters)

	FORMAT Chunk (24 bytes in length total) Byte Number
		0 - 3	"fmt_" (ASCII Characters)
		4 - 7	Length Of FORMAT Chunk (Binary, always 0x10)
		8 - 9	Always 0x01
		10 - 11	Channel Numbers (Always 0x01=Mono, 0x02=Stereo)
		12 - 15	Sample Rate (Binary, in Hz)
		16 - 19	Bytes Per Second
		20 - 21	Bytes Per Sample: 1=8 bit Mono, 2=8 bit Stereo or 16 bit Mono, 4=16 bit Stereo
		22 - 23	Bits Per Sample

  DATA Chunk Byte Number
		0 - 3	"data" (ASCII Characters)
		4 - 7	Length Of Data To Follow
		8 - end	Data (Samples)
*/


Cce152::Cce152(CPObject *parent)	: CPObject(parent)
{
   // if (parent == 0) pPC = (CPObject *)this;
    BackGroundFname	= ":/EXT/ext/ce-152.png";
    Tapein		= 0;				//Tape loaded (0:none, other:access)
    TapeCounter	= 0;
    mode		= EJECT;
    SoundOn		= FALSE;
    info.ptrFd	= 0;
    pTAPECONNECTOR	= new Cconnector(this,2,"Line in / Rec",false);	publish(pTAPECONNECTOR);
    pTIMER		= new Ctimer(this);
    Pc_DX		= 200;
    Pc_DY		= 320;
    KeyMap		= KeyMapce152;
    KeyMapLenght= KeyMapce152Lenght;
    pKEYB		= new Ckeyb(this,"ce152.map");
    first_state = 0;
    counter		= 0;
    GetWav_Val	= 0;
};


/*********************************/
/* Initialize Tape				 */
/*********************************/
bool Cce152::init(void)
{
    AddLog(LOG_TAPE,"Tape initializing...")

    info.ptrFd	= 0;
    mode	= EJECT;

    CPObject::init();

    setfrequency( 0);

    WatchPoint.add(&pTAPECONNECTOR_value,64,2,this,"Line In / Rec");

    if(pKEYB)	pKEYB->init();
    if(pTIMER)	pTIMER->init();

    return true;
}

/*****************************************************/
/* Exit TAPE										 */
/*****************************************************/
bool Cce152::exit(void)
{
    StopPlay();
	EjectTape();
	return true;
}

void Cce152::ComputeKey(void)
{
    BYTE k = pKEYB->LastKey;
    switch (k) {
    case K_PLAY : if (mode == LOAD) Play();
                    break;
    case K_EJECT:
        switch (mode) {
            case EJECT:
            case STOP : LoadTape(); break;
            case LOAD : LoadTape(); break;
            case PLAY : StopPlay(); break;
            case RECORD : StopPlay(); break;
		}
        break;
    case K_RECORD: RecTape(); break;
    }
}

bool Cce152::run(void)
{
	// Compute input pin
	SetWav(pTAPECONNECTOR->Get_pin(2));
	
	// Compute output pin
	pTAPECONNECTOR->Set_pin(1,GetWav());
	
	pTAPECONNECTOR_value = pTAPECONNECTOR->Get_values();
	return true;
}

bool Cce152::GetWav(void)
{
	float		ratio;
	qint64		new_state;
	qint64		delta_state;
	
	if (mode != PLAY) return false;			// Return 0 If not PLAY mode

    if (first_state == 0) {
        counter = 0;
        first_state = pTIMER->state;
    }

	new_state = pTIMER->state;
	delta_state = new_state - first_state;

	// Calculate nb of byte to skip corresponding to the CPU frequency
	int tmp = pTIMER->pPC->getfrequency();
	u_long tmp2= info.freq;
	qint64 wait = (counter * pTIMER->pPC->getfrequency() / info.freq);
    if (delta_state >= wait) {
		GetWav_Val = myfgetc(&info);
		counter++;
	}

	return ((GetWav_Val>0x10)?true:false);
}

int Cce152::myfgetc(WavFileInfo* ptrFile)
{
//	fprintf(fp_tape,"Read byte. - ");
	if (info.ptrFd)
	{
		int c=fgetc(ptrFile->ptrFd);
		if (c!=EOF)
		{
			TapeCounter++;
			return c;
		}
		else
			return 0;
	}
	else
		return 0;
}

bool Cce152::SetWav(bool bit)
{
	static BYTE	Val=0;
	float		ratio;
	qint64		new_state;
	qint64		delta_state;
	static qint64		counter = 0;
	
//	if (fp_tape==NULL) fp_tape=fopen("LOG TAPE.txt","wb");
	
//fprintf(fp_tape,"setwav - mode=%d",mode);
	if (mode != RECORD) return false;			// Return 0 If not PLAY mode
//fprintf(fp_tape,"RECORD - ");
//if (pTIMER->pPC->pCPU) fprintf(fp_tape," Xout=%d - ",pTIMER->pPC->pCPU->Get_Xout());

	if (first_state == 0) 
		{
			counter = 0;
//			delta = 0;
			first_state = pTIMER->state;
		}

	new_state = pTIMER->state;
	delta_state = new_state - first_state;

	// Calculate nb of byte to skip corresponding to the CPU frequency
	qint64 wait = (counter * pTIMER->pPC->getfrequency() / info.freq);
	if (delta_state >= wait)
	{
        int error = fputc ( (bit?0xFF:0x00), info.ptrFd) ;
		counter++;
	}

	return (true);
}

int	Cce152::Play(void)
{
	mode = PLAY;
	return 1;
}

int	Cce152::StopPlay(void)
{
	if (mode == RECORD)
	{
		UpdateHeadToWav (0,&info);
        fflush(info.ptrFd);
	}
	mode = STOP;
	return 1;
}

int	Cce152::EjectTape(void)
{
	if (info.ptrFd)
	{
        StopPlay();
		AddLog(LOG_TAPE,"Stop Play");
		fclose(info.ptrFd);
		info.ptrFd=0;
		TapeCounter = 0;
	}
	mode = EJECT;
	return 1;
}

int Cce152::LoadTape(void)
{
    int		error;
    long	nbByte ;


    // Display the Open dialog box.

    QString ofn = QFileDialog::getOpenFileName(
                    mainwindow,
                    "Choose a file",
                    ".",
                    "Wav Files (*.wav)");


    if (ofn.isEmpty())
    {
        mode = EJECT;
        return 0;
    }
    QFile file(ofn);

    char * str = qstrdup(ofn.toLocal8Bit());

    if ((info.ptrFd = fopen(str,"rb"))==NULL)
//	if (!file.open(QIODevice::ReadOnly))
    {
        MSG_ERROR(tr("Failed to open file"));
        return 0;
    }

    TapeFileName = ofn;

//	info.ptrFd = (FILE *) file.handle();

    /* Seek to the end of the source file */
    error = fseek (info.ptrFd, 0, SEEK_END) ;
    if (error != ERR_OK) {
        MSG_ERROR(tr("ERROR: Can't seek the file")) ;
        return 0;
    }

    /* Get the length of the source file */
    nbByte = ftell (info.ptrFd) ;
    if (nbByte == ERR_NOK) {
        MSG_ERROR(tr("ERROR: Can't ftell the file")) ;
        error = ERR_NOK ;
        return 0;
    }

    /* Seek to the begining of the source file */
    error = myfseek (&info, 0, SEEK_SET) ;
    if (error != ERR_OK) {
        MSG_ERROR(tr("ERROR: Can't seek the file")) ;
        return 0;
    }

    if (nbByte <= 58) {
        MSG_ERROR(tr("Source file is not a valid WAV File"));
        error = ERR_NOK ;
        return 0;
    }

    /* Read the header of the source WAV file */
    error = ReadWavHead (&info) ;
    if (error != ERR_OK) return 0;

    /* Check the lower frequency limit */
    if (info.freq < 5000) {
        AddLog(LOG_TAPE,tr("ERROR: Sampling frequency is lower than 5000 Hz : %1 Hz").arg(info.freq));
        MSG_ERROR(tr("Sampling frequency is lower than 5000 Hz"));
        error = ERR_NOK ;
        return 0;
    }

    /* Check the higher frequency limit */
    if (info.freq > 22050) {
        AddLog(LOG_TAPE,tr("ERROR: Sampling frequency is greater than 22050 Hz : %1 Hz").arg(info.freq));
        MSG_ERROR(tr("Sampling frequency is greater than 22050 Hz : %1 Hz").arg(info.freq));
        error = ERR_NOK ;
        return 0;
    }
//	MSG_ERROR(tr("Sampling frequency is  %1 Hz").arg(info.freq));

//	info.freq = (u_long) pPC->Tape_Base_Freq ;
    info.bitLen = (info.freq  * 8) / pTIMER->pPC->Tape_Base_Freq ;

    med_pt	= (info.freq/rec_speed)*0.45;
    low_pt	= (info.freq/rec_speed)*0.25;


    first_state = 0;
    mode = LOAD;
    return (1);
}

int Cce152::RecTape(void)
{

    // Display the Save dialog box.
    TapeFileName = QFileDialog::getSaveFileName(
                    mainwindow,
                    "Choose a filename to save under",
                    ".",
                    "Wav Files (*.wav)");
    QFile file(TapeFileName);

    char * str = qstrdup(TapeFileName.toLocal8Bit());

    //if (!file.open(QIODevice::WriteOnly))
    if ((info.ptrFd = fopen(str,"wb"))==NULL)
    {
        AddLog(LOG_TAPE,tr("ERROR: Can't create the wav file."));
        MSG_ERROR(tr("Failed to create file"));
        return 0;
    }

    // create th header
    WriteHeadToWav (10,
                    pTIMER->pPC->Tape_Base_Freq*2,
                    &info);
    info.bitLen = 0x10;
    info.freq = pTIMER->pPC->Tape_Base_Freq*2;
//       ptrFile->bitLen = (freq * 8) / ptrFile->freq ;


    mode = RECORD;

    return (1);
}


int Cce152::getTapeCounter(void)
{
    return TapeCounter;
}

int Cce152::myfseek( WavFileInfo* ptrFile , long offset , int origin )
{

    switch (origin)
    {
    case SEEK_SET: TapeCounter=offset; break;
    case SEEK_CUR: TapeCounter+=offset;break;
    }

    return fseek (ptrFile->ptrFd, offset, origin);
}

void Cce152::CvStringIToShort (char* ptrStr,u_short* ptrVal)
{
    /* Convert the String to a short value with msb first (INTEL) */
    *ptrVal =  (u_short) ptrStr[0] & 0xFF ;
    *ptrVal += ((u_short) ptrStr[1] & 0xFF) << 8 ;
}

void Cce152::CvStringIToLong (char*   ptrStr,u_long* ptrVal)
{

    /* Convert the String to a long value with msb first (INTEL) */
    *ptrVal =  (u_short) ptrStr[0] & 0xFF ;
    *ptrVal += ((u_short) ptrStr[1] & 0xFF) << 8 ;
    *ptrVal += ((u_short) ptrStr[2] & 0xFF) << 16 ;
    *ptrVal += ((u_short) ptrStr[3] & 0xFF) << 24 ;
}

int Cce152::ReadStringFromFile (char*  ptrStr,long   nb,FILE** ptrFd)
{
    int         ii ;
    int         inVal ;
    int         error ;

    error = ERR_OK ;

    for ( ii = 0 ; ii < nb ; ii++  ) {
        inVal = fgetc (*ptrFd) ;
        if (inVal == EOF) {
            printf ("ERROR : Can't read over End Of File\n") ;
            error = ERR_NOK ;
            break ;
        }
        *ptrStr++ = (char) inVal ;
    }
    *ptrStr = 0 ;
    return (error);
}

int Cce152::ReadLongFromFile (u_long* ptrVal,FILE**  ptrFd)
{
    char        str[10] ;
    int         error ;

    error = ReadStringFromFile (str, 4, ptrFd) ;
    CvStringIToLong (str, ptrVal) ;

    return (error);
}

int Cce152::ReadShortFromFile (u_short* ptrVal,FILE**   ptrFd)
{
    char        str[10] ;
    int         error ;

    error = ReadStringFromFile (str, 2, ptrFd) ;
    CvStringIToShort (str, ptrVal) ;

    return (error);
}



int Cce152_PC15XX::SetBit(bool)
{
    return(1);
}

int Cce152_PC15XX::serial(int bit)
{
	QTime time;
	static qint64 last_state = 0;
	static DWORD last_time=0;

	static int c=0,waitbitstart=1,waitbitstop=0;
	static long timewait=0,startstate=0;
	static int bit_number = 0;
	static DWORD Byte = 0;
	DWORD	Byte2;
	int		bit_number2;
	BYTE	t=0;

//	0	START BIT
//	0-1	b4 0=set,1=not set	
//	0-1	b5 0=set,1=not set
//	0-1	b6 0=set,1=not set
//	0-1	b7 0=set,1=not set
//	1	STOP BIT
//	1	STOP BIT
//	1	STOP BIT
//	1	STOP BIT
//	1	STOP BIT
//	1	STOP BIT
//	0	START BIT
//	0-1	b0 0=set,1=not set
//	0-1	b1 0=set,1=not set
//	0-1	b2 0=set,1=not set
//	0-1	b3 0=set,1=not set
//	1	STOP BIT
//	1	STOP BIT
//	1	STOP BIT
//	1	STOP BIT
//	1	STOP BIT
//	1	STOP BIT

	if (last_state == 0) last_state = pPC->pTIMER->state;

	if (last_time==0) 
		time.restart(); //last_time=timeGetTime();

	if (fp_tape==NULL) 
		fp_tape=fopen("LOG TAPE.txt","wb");

	if ((bit_number == 0) && (bit !=0)) return 0;
	
	if (bit) Byte |= (0x01 << bit_number);

	fprintf(fp_tape,"%c",(bit==1?'1':'0'));

	Byte2=Byte;
	bit_number++;
	
	
	bit_number2=bit_number;

	if (bit_number==22)
	{
		// Fin
		// compute l'octet
		t=((Byte >> 12) & 0x0F);
		t|= (((Byte>>1)&0x0F)<<4);

		t=((t>>4)&0x0F) | ((t&0x0F)<<4);
		
		DWORD delta_time = time.restart();//-- timeGetTime() - last_time;
		fprintf(fp_tape,"    %02X=%c  Tape Counter=%d  time=%d   delta_state=%d\n",t,t,TapeCounter,delta_time,pPC->pTIMER->state-last_state);
//--		AddLog(LOG_TAPE,tr("Lecture K7: 0x%1 = %c",t,t);
//--		last_time=timeGetTime();
		last_state = pPC->pTIMER->state;
		// reinit var
		bit_number = 0;
		Byte=0;
	}
	return 1;
};

int Cce152_PC15XX::GetBit(void)
{
	static qint64 last_state = 0;
	int			nb_byte;
	static int	Byte_read = 0;
	float		ratio;
	qint64		new_state;
	qint64		delta_state;
    u_long		trans ;
	static char		bit=1;
	static int last_TapeCounter=0;

	if (mode != PLAY) return 1;			// Return 0 If not PLAY mode
	


//	ce152.info.debug=0x04;
	if (fp_tape==NULL) fp_tape=fopen("LOG TAPE.txt","wb");

	if (last_TapeCounter==0) last_TapeCounter=TapeCounter;

	if (last_state == 0) last_state = pTIMER->state;

	new_state = pTIMER->state;
	delta_state = new_state - last_state;

	// Calculate nb of byte to skip corresponding to the CPU frequency

	ratio = (float) delta_state/pTIMER->pPC->getfrequency()*2;
	nb_byte = (int) (info.freq * ratio);

	if (nb_byte)
	{
		if (nb_byte > Byte_read)
		{
			ReadBitFromWav(&trans,&info);
			if (trans > BIT_MID)
				bit = 1;     /* Bit a 1 */
			else
				bit = 0;     /* Bit a 0 */
			Byte_read=TapeCounter-last_TapeCounter;//;ce152.info.bitLen;
			int log1=Byte_read;
			int log2=last_TapeCounter;

			carac=trans;
			serial(bit);

		}
		last_state = new_state;

	}

	Byte_read-=nb_byte;
	int log3=Byte_read;
	if (Byte_read==0)
	{
		last_TapeCounter=TapeCounter;
	}

	return bit;
}

void Cce152_PC15XX::AddToWavBuffer(char c)
{
	static int BufPos=0;

	WavBuffer[BufPos] = c;
	BufPos++;

	if (BufPos >= (info.bitLen*100))
	{
		
//--		if (SoundOn) writeAudioBlock(hWaveOut, &WavBuffer[0], info.bitLen*100);
		BufPos = 0;
	}
}

int Cce152_PC15XX::getperiod()
{
	int l;

	do
	{
		l=1;
		if (carac>127)
			while ( ((carac=myfgetc(&info))>127) && (carac != EOF) )
				l++;
		else
			while ( ((carac=myfgetc(&info))<128) && (carac != EOF) )
				l++;
	} while ( (l<low_pt) && (carac != EOF) );

	if ( carac == EOF ) return (EOF);
	return l;
}

bool Cce152_PC15XX::getbit(void)
{
	int temp = getperiod();
	if (temp > med_pt) 
		return 0;
	else
	{
		if (temp == EOF) return (EOF);
		return 1;
	}
}

BYTE Cce152_PC15XX::getbyte(void)
{
  int i,octet=0;

  for (i=0;i<8;i++)
    octet|=getbit() << (7-i);


  return octet;
}

int Cce152_PC15XX::ReadBitFieldFromWav (u_long nbBits, char* ptrBits, WavFileInfo* ptrFile)
{
    int     ii ;
    u_long  trans ;
    u_long  limit ;
    int     error=0 ;

    if ( (ptrFile->debug & 0x0004) > 0 )
        printf (" Bits") ;

    for ( ii = 0 ; ii < nbBits ; ii++ ) {
        error = ReadBitFromWav (&trans, ptrFile);
        if (error != ERR_OK) break ;

        if (trans > BIT_MID) {
            if (trans < BIT_1) {
                limit = ptrFile->bitLen ;
                error = SyncBitFromWav (1, &trans, &limit, ptrFile);
                if (error != ERR_OK) break ;
                if (limit == 0) {
                    fprintf (fp_tape,"ERROR : Synchro lost %d\n",trans) ;
                    error = ERR_NOK ;
                    break ;
                }
             }
        }
        else {
            if (trans > BIT_0 ) {
                limit = ptrFile->bitLen ;
                error = SyncBitFromWav (-1, &trans, &limit, ptrFile);
                if (error != ERR_OK) break ;
                if (limit == 0) {
                    fprintf (fp_tape,"ERROR : Synchro lost %d\n",trans) ;
                    error = ERR_NOK ;
                    break ;
                }
             }
        }
        if (error != ERR_OK) break ;

        if (trans > BIT_MID)
            ptrBits[ii] = 1;     /* Bit a 1 */
        else
            ptrBits[ii] = 0;     /* Bit a 0 */

        if ( (ptrFile->debug & 0x0004) > 0 )
            fprintf (fp_tape," %d", ptrBits[ii]) ;
    }
        if ( (ptrFile->debug & 0x0004) > 0 )
            fprintf (fp_tape,"\n") ;

    return (error);
}

int Cce152_PC15XX::SkipBitsFromWav (u_long*   ptrNb, WavFileInfo* ptrFile)
{
    int     ii ;
    char    bit ;
    int     error ;

    do {
        *ptrNb = 0 ;
        ii = 0 ;
        do {
            ii++ ;
            error = ReadBitFieldFromWav (1, &bit, ptrFile) ;
            if (error != ERR_OK) break ;

        } while (bit > 0) ;
        if (error != ERR_OK) break ;

        ii-- ;
        error = myfseek (ptrFile, - ptrFile->bitLen, SEEK_CUR) ;
        if (error != ERR_OK) {
            AddLog(LOG_TAPE,"ERROR : Can't seek the file\n") ;
            break ;
        }

        *ptrNb = ii ;
        if ( (ii > 0) && ((ptrFile->debug & 0x0008) > 0 ) )
            AddLog(LOG_TAPE,tr(" Skip %1").arg(ii) );

    } while (0) ;

    return (error);
}

int Cce152_PC15XX::SyncBitFromWav (int sign, u_long*   ptrTrans, u_long* ptrLimit, WavFileInfo* ptrFile)
{
//    int         inVal ;
    int         ii ;
    int         tmp ;
    int         error ;

    if ( (ptrFile->debug & 0x0002) > 0 )
        fprintf(fp_tape," %d -> ", *ptrTrans);

    ii    = 1 ;
    do {
        if ( (ii % 2) == 0 )
            tmp = - ptrFile->bitLen + ii ;
        else
            tmp = - ptrFile->bitLen - ii ;

        error = myfseek (ptrFile, tmp, SEEK_CUR) ;
        if (error != ERR_OK) {
            fprintf (fp_tape,"ERROR : Can't seek the file\n") ;
            break ;
        }
        error = ReadBitFromWav (ptrTrans, ptrFile);
        if (error != ERR_OK) break ;

        ii++;

    } while ( (ii < *ptrLimit) &&
              ( ( (sign > 0) && (*ptrTrans < BIT_1) ) ||
                ( (sign < 0) && (*ptrTrans > BIT_0) ) ) ) ;

    if ( (ptrFile->debug & 0x0002) > 0 ) {
        fprintf(fp_tape,"%d bits -> ", tmp + ptrFile->bitLen);
        fprintf(fp_tape,"%d\n", *ptrTrans);
    }

    if ( (*ptrTrans < BIT_1) &&
         (*ptrTrans > BIT_0) )
        *ptrLimit = 0 ;
    else
        *ptrLimit = ii ;

    return (error);
}

int Cce152_PC15XX::ReadBitFromWav (u_long*   ptrTrans,WavFileInfo* ptrFile)
{
    int         sign ;
    int         inVal ;
    int         ii ;
    int         error ;

    error = ERR_OK ;
    *ptrTrans = 0;
    sign = 1;
    for ( ii = 0 ; ii < ptrFile->bitLen ; ii++ ) {
        inVal = myfgetc (ptrFile) ; 
        if (inVal == EOF) {
            fprintf (fp_tape,"\nEnd of File\n") ;
            error = ERR_NOK ;
            break ;
        }
        if (sign > 0) {
            if (inVal > TRANS_HIGH) {
                continue ;
            }
            if (inVal < TRANS_LOW) {
                sign = -1;
                *ptrTrans = *ptrTrans + 1 ;
            }
        }
        else {
            if (inVal < TRANS_LOW) {
                continue ;
            }
            if (inVal > TRANS_HIGH) {
                sign = 1;
                *ptrTrans = *ptrTrans + 1 ;
            }
        }
    }

//    if ( (ptrFile->debug & 0x0001) > 0 )
//       fprintf(fp_tape," %d", *ptrTrans);

    return (error);
}

int Cce152_PC15XX::ReadQuaterFromWav (u_long*   ptrQuat, WavFileInfo* ptrFile)
{
    u_long  code ;
    char    bit[5] ;
    int     error ;

    do {
        *ptrQuat = 0 ;
        error = ReadBitFieldFromWav (5, bit, ptrFile) ;
        if (error != ERR_OK) break ;

        code = (u_long) bit[4] ;
        code = (code << 1) + (u_long) bit[3] ;
        code = (code << 1) + (u_long) bit[2] ;
        code = (code << 1) + (u_long) bit[1] ;
        *ptrQuat = code ;

        if ( (ptrFile->debug & 0x0010) > 0 )
            printf(" %1X", code);

        error = SkipBitsFromWav (&code, ptrFile);
        if (error != ERR_OK) break ;

    } while (0) ;

    return (error);
}

int Cce152_PC15XX::ReadByteFromWav (u_long    order, u_long*   ptrByte, WavFileInfo* ptrFile)
{
    u_long  lsq ;
    u_long  msq ;
    int     error ;

	if (mode != PLAY) return 0;			// Return 0 If not PLAY mode

    do {
        *ptrByte = 0 ;
        if (order == ORDER_INV) {
            error = ReadQuaterFromWav (&lsq, ptrFile) ;
            if (error != ERR_OK) break ;
            error = ReadQuaterFromWav (&msq, ptrFile) ;
            if (error != ERR_OK) break ;
        }
        else {
            error = ReadQuaterFromWav (&msq, ptrFile) ;
            if (error != ERR_OK) break ;
            error = ReadQuaterFromWav (&lsq, ptrFile) ;
            if (error != ERR_OK) break ;
        }

        *ptrByte = (msq << 4) + lsq ;
//        if ( (ptrFile->debug & 0x0020) > 0 )
//--            AddLog(LOG_TAPE,"read: %02X=%c", *ptrByte, *ptrByte);

    } while (0) ;

    return (error);
}

int Cce152_PC15XX::FindSyncFromWav (u_long freq,u_long* ptrNb,WavFileInfo*  ptrFile)
{
    int         ii ;
    u_long      trans ;
    u_long      limit ;
    int         error ;

    do {
        /* Calculate the number of WAV bytes per bit */
        ptrFile->bitLen = (freq * 8) / ptrFile->freq ;

        *ptrNb = 0 ;
        ii = 0;
        do {
            ii++;
            do {
                error = ReadBitFromWav (&trans, ptrFile);
                if (error != ERR_OK) break ;

                if ( (ptrFile->debug & 0x0100) > 0 )
                    printf (" Search %d\n", trans) ;

            } while (trans < (BIT_MID / 2) ) ;
            if (error != ERR_OK) break ;

            limit = ptrFile->bitLen ;
            error = SyncBitFromWav (1, &trans, &limit, ptrFile);
            if (error != ERR_OK) break ;

            if ( (ptrFile->debug & 0x0100) > 0 )
                printf (" Search %d\n", trans) ;

        } while ( (trans < BIT_1) && (ii < SYNC_NB_TRY) ) ;
        if (ii >= SYNC_NB_TRY) break;

        /* Skip the Synchro */
        error = SkipBitsFromWav (ptrNb, ptrFile);
        if (error != ERR_OK) break ;

    } while (0) ;

    return (error);
}

int Cce152_PC15XX::FindFreqAndSyncFromWav (u_long freq,WavFileInfo*  ptrFile)
{
    u_long      length ;
    int         error=0 ;

    do {
        if (freq >= (BASE_FREQ1 * 2)) {
            /* Search the Synchro for the 4000 Hz frequency */
            ptrFile->freq = BASE_FREQ1 ;
            error = FindSyncFromWav (freq, &length, ptrFile) ;
            if (error != ERR_OK) break ;
        }
        else
            length = 0 ;

        if (length == 0) {
            /* Search the Synchro for the 2500 Hz frequency */
            ptrFile->freq = BASE_FREQ2 ;
            error = FindSyncFromWav (freq, &length, ptrFile) ;
            if (error != ERR_OK) break ;
        }

        if (length > 0) {
            length = (length * 8 + ptrFile->freq / 2) / ptrFile->freq ;
//--            AddLog(LOG_TAPE,"Synchro found : %d seconds at %d Hz", length, ptrFile->freq);
        }
        else {
            AddLog(LOG_TAPE,"ERROR : No Synchro found\n");
            error = ERR_NOK ;
            break ;
        }

    } while (0) ;

    return (error);
}

int Cce152::ReadWavHead (WavFileInfo* ptrFile)
{
    char        str[20] ;
    u_long      tmpL ;
    u_short     tmpS ;
    int         error ;

    do {
        error = ReadStringFromFile (str, 4, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;
        if (strcmp (str, "RIFF") != 0) {
            AddLog(LOG_TAPE,"ERROR: Header isn't 'RIFF'") ;
            error = ERR_NOK ;
            break ;
        }

        error = ReadLongFromFile (&tmpL, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;

        error = ReadStringFromFile (str, 8, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;
        if (strcmp (str, "WAVEfmt ") != 0) {
            AddLog(LOG_TAPE,"ERROR: Header format isn't 'WAVEfmt '") ;
            error = ERR_NOK ;
            break ;
        }

        error = ReadLongFromFile (&tmpL, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;

        error = ReadShortFromFile (&tmpS, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;
        if (tmpS != 1) {
            AddLog(LOG_TAPE,"ERROR: Format isn't PCM") ;
            error = ERR_NOK ;
            break ;
        }

        error = ReadShortFromFile (&tmpS, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;
        if (tmpS != 1) {
            AddLog(LOG_TAPE,"ERROR: Format isn't Mono") ;
            error = ERR_NOK ;
            break ;
        }

        error = ReadLongFromFile (&ptrFile->freq, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;
        error = ReadLongFromFile (&tmpL, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;

        error = ReadShortFromFile (&tmpS, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;
        if (tmpS != 1) {
            AddLog(LOG_TAPE,"ERROR: Format isn't 1 byte / sample") ;
            error = ERR_NOK ;
            break ;
        }

        error = ReadShortFromFile (&tmpS, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;
        if (tmpS != 8) {
            AddLog(LOG_TAPE,"ERROR: Format isn't 8 bits / sample") ;
            error = ERR_NOK ;
            break ;
        }

        error = ReadStringFromFile (str, 4, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;
        if (strcmp (str, "data") != 0) {
            error = myfseek (ptrFile, -2, SEEK_CUR) ;
            if (error != ERR_OK) {
                AddLog(LOG_TAPE,"ERROR: Can't seek the file") ;
                break ;
            }

            error = ReadStringFromFile (str, 4, &ptrFile->ptrFd) ;
            if (error != ERR_OK) break ;
            if (strcmp (str, "data") != 0) {
                error = ReadStringFromFile (str, 8, &ptrFile->ptrFd) ;
                if (error != ERR_OK) break ;

                error = ReadStringFromFile (str, 4, &ptrFile->ptrFd) ;
                if (error != ERR_OK) break ;

                if (strcmp (str, "data") != 0) {
                    AddLog(LOG_TAPE,"ERROR: Data Header isn't 'data'") ;
                    error = ERR_NOK ;
                    break ;
                }
            }
        }

        error = ReadLongFromFile (&tmpL, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;

    } while (0) ;
    return (error);
}

int Cce152::WriteHeadToWav (u_long    nbSamp,
                    u_long    freq,
                    WavFileInfo* ptrFile)
{
    int     error ;

    do {
        error = WriteStringToFile ("RIFF", &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;

        error = WriteLongToFile ((nbSamp + 36), &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;

        error = WriteStringToFile ("WAVEfmt ", &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;

        error = WriteLongToFile (0x10, &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;

        error = WriteShortToFile (1, &ptrFile->ptrFd) ;      /* PCM */
        if (error != ERR_OK) break ;

        error = WriteShortToFile (1, &ptrFile->ptrFd) ;      /* Mono */
        if (error != ERR_OK) break ;

        error = WriteLongToFile (freq, &ptrFile->ptrFd) ;    /* Samp Freq */
        if (error != ERR_OK) break ;

        error = WriteLongToFile (freq, &ptrFile->ptrFd) ;    /* Byte / sec */
        if (error != ERR_OK) break ;

        error = WriteShortToFile (1, &ptrFile->ptrFd) ;      /* Byte / Samp x Chan */
        if (error != ERR_OK) break ;

        error = WriteShortToFile (8, &ptrFile->ptrFd) ;      /* Bit / Samp */
        if (error != ERR_OK) break ;

        error = WriteStringToFile ("data", &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;

        error = WriteLongToFile (nbSamp, &ptrFile->ptrFd) ;  /* Nb Samples */
        if (error != ERR_OK) break ;

    } while (0) ;
    return (error);
}

int ReadFileLength (u_long* ptrLen,
                    FILE**  ptrFd)
{
    long    nbByte ;
    int     error ;

    do {
        *ptrLen = 0 ;

        /* Seek to the end of the source file */
        error = fseek (*ptrFd, 0, SEEK_END) ;
        if (error != ERR_OK) {
            printf ("ERROR : Can't seek the file\n") ;
            break ;
        }

        /* Get the length of the source file */
        nbByte = ftell (*ptrFd) ;
        if (nbByte == ERR_NOK) {
            printf ("ERROR : Can't ftell the file\n") ;
            error = ERR_NOK ;
            break ;
        }

        /* Seek to the begining of the source file */
        error = fseek (*ptrFd, 0, SEEK_SET) ;
        if (error != ERR_OK) {
            printf ("ERROR : Can't seek the file\n") ;
            break ;
        }

        if (nbByte == 0) {
            printf ("ERROR : Source file is empty\n") ;
            error = ERR_NOK ;
            break ;
        }

        *ptrLen = nbByte ;

    } while (0) ;
    return (error);
}

int Cce152::UpdateHeadToWav (u_long    nbSamp,WavFileInfo* ptrFile)
{
    int     error ;


    do {
		u_long	Length;
		error=ReadFileLength (&Length,&ptrFile->ptrFd);
AddLog(LOG_TAPE,tr("Length=%1").arg(Length));
		error = fseek (ptrFile->ptrFd, 4, SEEK_SET) ;

        error = WriteLongToFile ((Length - 8), &ptrFile->ptrFd) ;
        if (error != ERR_OK) break ;

		error = fseek (ptrFile->ptrFd, 40, SEEK_SET) ;

        AddLog(LOG_TAPE,"ERROR");
        error = WriteLongToFile (Length - 44, &ptrFile->ptrFd) ;  /* Nb Samples */
        if (error != ERR_OK) break ;
AddLog(LOG_TAPE,"ERROR");
    } while (0) ;
    return (error);
}

int Cce152_PC15XX::WriteBitToWav (u_long    value,
                   WavFileInfo* ptrFile)
{
    int     ii ;
    int     error=0 ;

	if (ptrFile->ptrFd==0) return (0);

    for ( ii = 0 ; ii < 16 ; ii++ ) {
        error = fputc (bitwav[value][ii], ptrFile->ptrFd) ;
		AddToWavBuffer(bitwav[value][ii]);
//		AddLog(LOG_TAPE,"%02X",bitwav[value][ii]);
        if (error == EOF) {
            printf ("ERROR : Can't write in the file\n") ;
            error = ERR_NOK ;
            break ;
        }
        else
            error = ERR_OK ;
    }
    return (error);
}

int Cce152::WriteStringToFile (char*  ptrStr,
                       FILE** ptrFd)
{
    int     error ;

    error = fputs (ptrStr, *ptrFd) ;
    if (error == EOF) {
        printf ("ERROR : Can't write in the file\n") ;
        error = ERR_NOK ;
    }
    else {
        error = ERR_OK ;
    }
    return (error);
}

int Cce152::WriteLongToFile (u_long value,
                     FILE** ptrFd)
{
    unsigned char    str[10] ;
    int     ii ;
    int     error =0;

    CvLongToStringI (value, str) ;

    for ( ii = 0 ; ii < 4 ; ii++  ) {
        error = fputc (str[ii], *ptrFd) ;
        if (error == EOF) {
            printf ("ERROR : Can't write in the file\n") ;
            error = ERR_NOK ;
            break ;
        }
        else
            error = ERR_OK ;
    }
    return (error);
}

int Cce152::WriteShortToFile (u_long value,
                      FILE** ptrFd)
{
    char    str[10] ;
    int     ii ;
    int     error=0 ;

    CvShortToStringI (value, str) ;

    for ( ii = 0 ; ii < 2 ; ii++ ) {
        error = fputc (str[ii], *ptrFd) ;
        if (error == EOF) {
            printf ("ERROR : Can't write in the file\n") ;
            error = ERR_NOK ;
            break ;
        }
        else
            error = ERR_OK ;
    }
    return (error);
}

void Cce152::CvShortToStringI (u_short value,
                       char*   ptrStr)
{
    u_short tmp ;

    /* Convert the short value into a String with msb first (INTEL) */
    tmp = value & 0xFF ;
    *ptrStr ++ = (char) tmp ;
    tmp = value >> 8 ;
    *ptrStr ++ = (char) tmp ;
    *ptrStr = 0 ;
}

void Cce152::CvLongToStringI (u_long value,
                      unsigned char*  ptrStr)
{
    u_long  tmp ;

    /* Convert the long value into a String with msb first (INTEL) */
    tmp = value & 0xFF ;
    *ptrStr ++ = (unsigned char) tmp ;
    tmp = (value >> 8) & 0xFF ;
    *ptrStr ++ = (unsigned char) tmp ;
    tmp = (value >> 16) & 0xFF ;
    *ptrStr ++ = (unsigned char) tmp ;
    tmp = value >> 24 ;
    *ptrStr ++ = (unsigned char) tmp ;
    *ptrStr = 0 ;
}

int Cce152_PC15XX::RecTape(void)
{
	Cce152::RecTape();
	int sync = (2 * pTIMER->pPC->Tape_Base_Freq) / 8 ;
	WriteSyncToWav (sync, &info) ;
	return (1);
}

int Cce152_PC15XX::WriteSyncToWav (int       nbSync,
                    WavFileInfo* ptrFile)
{
    int     ii ;
    int     error=0 ;

    do {
        /* Write the Synchro patern */
        for ( ii = 0 ; ii < nbSync ; ii++ ) {
            error = WriteBitToWav (1, ptrFile) ;
            if (error != ERR_OK) break ;
        }
        if (error != ERR_OK) break ;

    } while (0) ;
    return (error);
}

