#ifndef _EXTENSION_H
#define _EXTENSION_H

#include <QString>
#include <QAction>
#include <QMenu>

#define ID_CE151	0
#define ID_CE153	1
#define ID_CE155	2
#define ID_CE156	3
#define ID_CE157	4
#define ID_CE158	5
#define ID_CE159	6
#define ID_CE160	7
#define ID_CE161	8
#define ID_CE201M	9
#define ID_CE202M	10
#define ID_CE203M	11
#define ID_CE210M	12
#define ID_CE211M	13
#define ID_CE212M	14
#define ID_CE2H16M	15
#define ID_CE2H32M	16
#define ID_CE2H64M	17


class CExtension{
public:
	char*	GetClassName(){ return("CExtension"); };

	QString	Id;
	QString Description;
	bool	IsAvailable;
	bool	IsChecked;
	bool	needreboot;
	QString fname;
	int		DX,DY;
	int		offset_pcX,offset_pcY;
	
	QAction *Action;
	
	CExtension(void);			//[Basic constructor] {"","",false,false,true}
	CExtension(QString,QString,QString,int,int,int,int,bool,bool,bool);

};

class CExtensionArray{
public:
	char*	GetClassName(){ return("CExtensionArray"); };
	
	QString Id;
	QString Description;
	QMenu	*Menu;
	QAction *emptyAction;

	QActionGroup *actionGroup;

	CExtension	*ExtArray[30];

	void setAvailable(int ind,bool value);
	void setChecked(int ind,bool value);


	CExtensionArray(void);							//[constructor]
	CExtensionArray(QString,QString);							//[constructor]
	~CExtensionArray(void)							//[constructor]
	{
		delete Menu;
		delete emptyAction;
		delete actionGroup;
		for (int i=0;i<30;i++) delete ExtArray[i];
		
	}
};



#endif
