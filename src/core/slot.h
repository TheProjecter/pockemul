#ifndef _SLOT_H_
#define _SLOT_H_

#include <qstring.h>

class CSlot 
{
public:
	CSlot(	int size, int adr, QString	resid, QString filename, int type, QString label)
	{
		Size = size;
		Adr	= adr;
		ResID = resid;
		FileName = filename;
		Type = type;
		Label = label;		
	};
	
	int		getSize()		  { return Size; }
	void	setSize(int size) { Size = size; };	
	
	int		getAdr()		{ return Adr; }
	void	setAdr(int adr)	{ Adr = adr; };
	
	QString getResID() { return ResID; }
	void	setResID(QString resid)	{ ResID = resid; };
	
	QString getFileName() { return FileName; }
	void	setFileName(QString filename)	{ FileName = filename; };
	
	QString getLabel() { return Label; }
	void	setLabel(QString label)	{ Label = label; };

	int		getType() { return Type; }
	void	setType(int type)	{ Type = type; };
	
private:
	int		Size;
	int		Adr;
	QString	ResID;
	QString	FileName;
	int		Type;
	QString	Label;
	
};
#endif
