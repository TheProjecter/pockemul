#include "xmlwriter.h"
 
#include "common.h"
#include "init.h"
#include "pcxxxx.h"
#include "Log.h"
#include "Inter.h"
#include "Keyb.h"

#include "Keyb1250.h"
#include "Keyb1350.h"
#include "Keyb1360.h"
#include "Keyb1401.h"
#include "Keyb1403.h"
#include "Keyb1450.h"
#include "Keyb1500.h"
#include "Keyb1600.h"
#include "keybce150.h"
#include "keybce152.h"


extern CPocketThread* PcThread;
INLINE BYTE bit(int ii)
{
    BYTE jj;

    for (jj= 0; jj < 8; jj++)
	{
		if (ii & 0x01) return(jj);
		ii >>= 1;
    }
    return(jj);
}

typedef struct{
	BYTE pc1350,scan;
}	Transkey;

const int RIGHT[9] = { 25 , 32 , 59 , 19, 21, 13, 21, 16, 31 };
const int BOTTOM[9]= { 16 , 22 , 16 , 21, 10, 13, 13, 31, 31 };

CKey::CKey(int scancode, QString description,QRect rect )	
{
	Description = description;
	ScanCode	= scancode;
	Rect = rect;
}

BYTE Ckeyb::KeyClick(QPoint pts)
{
	// Keys iterator 
	QList<CKey>::iterator i;
 	for (i = Keys.begin(); i != Keys.end(); ++i)
 	{
		if ( i->Rect.contains(pts) ) return i->ScanCode;
	}
	return(0);
}

QString Ckeyb::KeyString(QPoint pts)
{
	// Keys iterator 
	QList<CKey>::iterator i;
 	for (i = Keys.begin(); i != Keys.end(); ++i)
 	{
		if ( i->Rect.contains(pts) ) return i->Description;
	}
	return("");
}

void Ckeyb::keyscan(void)
{
    int i, j;
	unsigned char ch;

	for (i = 0; i < NM; i++)	{ keym[i] = 0; }
		
    for (i = 0; i < NM; i++)
	{
		for (j = 0; j < 8; j++)
		{
			ch = scandef[i * 8 + j];
			if ( (LastKey == ch) || (toupper(LastKey) == toupper(ch)) )
			{
				keym[i] = (1 << j);
				LastKey = 0;
				break;
			}	    
		}
		if (keym[i] != 0) break;
    }
}


BYTE Ckeyb::Read(BYTE data)
{
	BYTE	ret	=0;
	BYTE	jj	=0;

    if ((data == 0) && ((KStrobe & 0x7F) != 0))
	{
		jj = bit(KStrobe & 0x7F);
		if (jj < 7) { ret = keym[jj]; }
    }
    else 
	{
		jj = bit(data);
		if (jj < 8) { ret = keym[8 + jj]; }
    }
	return(ret);  
}

BYTE Ckeyb::Get_KS(void)
{
	return(KStrobe);
}

void Ckeyb::Set_KS(BYTE data)
{
	KStrobe = data;
}

bool Ckeyb::CheckKon()
{
	Kon = FALSE;
	if ( (LastKey == K_BRK) )
	{
		Kon = TRUE;
		AddLog(2,"Kon TRUE");
		LastKey = 0;
    }
	return (Kon);
}	

int Ckeyb::CheckOff()
{
	return(0);
}	

bool Ckeyb::init(void)
{
	QFile file( fn_KeyMap );
	QXmlInputSource source(&file);

	QXmlSimpleReader reader;
	reader.setContentHandler( handler );

	bool result = reader.parse( source );

	if (result) return true;

	// else load the struct
	for (int i = 0;i < pPC->KeyMapLenght;i++)
	{
		Keys.append(CKey(pPC->KeyMap[i].ScanCode,
						pPC->KeyMap[i].KeyDescription,
						QRect(	pPC->KeyMap[i].x1,
								pPC->KeyMap[i].y1,
								RIGHT[ pPC->KeyMap[i].Type - 1 ],
								BOTTOM[pPC->KeyMap[i].Type - 1 ])
						)
					);
	}
	return true;
}

bool Ckeyb::exit(void)
{
	if (! modified) return true;
		
	 int ret = QMessageBox::warning(mainwindow, tr("PockEmul"),
                   tr("The keyboard layout has been modified.\n"
                      "Do you want to save your changes?"),
                   QMessageBox::Save | QMessageBox::Discard,
                   QMessageBox::Save);
                   
    if (ret == QMessageBox::Discard) return true;

	// save the XML definition file
    QFile file(fn_KeyMap);

    file.open( QIODevice::WriteOnly);
    XmlWriter xw( &file );
    xw.setAutoNewLine( true );
    xw.writeRaw( "<!DOCTYPE KEYBMAP><KEYBMAP version=\"1.0\">" );
    xw.newLine();
    xw.writeTaggedString( "model", QString(pPC->getcfgfname()) );
    xw.writeOpenTag( "Keyboard" );
	
	QList<CKey>::iterator j;

	for (j = Keys.begin(); j != Keys.end(); ++j)
	{
    	AttrMap attrs;
    	attrs.insert( "description", j->Description.trimmed () );
    	attrs.insert( "scancode", QString("0x%1").arg(j->ScanCode,0,16) );
    	attrs.insert( "left", QString("%1").arg(j->Rect.left()) );
    	attrs.insert( "top", QString("%1").arg(j->Rect.top() ));
    	attrs.insert( "width", QString("%1").arg(j->Rect.width() ) );
    	attrs.insert( "height", QString("%1").arg(j->Rect.height() ) );
    	xw.writeAtomTag( "KEY", attrs );
    }

    xw.writeCloseTag( "Keyboard" );
    xw.writeRaw( "</KEYBMAP>" );

	return true;
}

bool KEYBMAPParser::startDocument()
{
	inKeyboard = false;
	return true;
}

bool KEYBMAPParser::endElement( const QString&, const QString&, const QString &name )
{
	if( name == "Keyboard" )
		inKeyboard = false;

   return true;
}

bool KEYBMAPParser::startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs )
{
	QString desc = "";
	int scancode,x,y,w,h;
	bool ok = false;
	
	scancode=x=y=w=h=0;
	
	if( inKeyboard && name == "KEY" )
	{
		
		for( int i=0; i<attrs.count(); i++ )
		{
			if( attrs.localName( i ) == "description" )
				desc = attrs.value( i );
			else if( attrs.localName( i ) == "scancode" )
				scancode = attrs.value( i ).toInt(&ok,16);
			else if( attrs.localName( i ) == "left" )
				x = attrs.value( i ).toInt(&ok,10);
			else if( attrs.localName( i ) == "top" )
				y = attrs.value( i ).toInt(&ok,10);
			else if( attrs.localName( i ) == "width" )
				w = attrs.value( i ).toInt(&ok,10);
			else if( attrs.localName( i ) == "height" )
				h = attrs.value( i ).toInt(&ok,10);
		}
		((Ckeyb*) Parent)->Keys.append(CKey(scancode,desc,QRect(x,y,w,h)));
		AddLog(LOG_KEYBOARD,mainwindow->tr("XML Read key : %1, scan=0x%2 , Rect=(%3,%4,%5,%6)").arg(desc).arg(scancode,0,16).arg(x).arg(y).arg(w).arg(h));
	}
	else if( name == "Keyboard" )
		inKeyboard = true;

	return true;
}
