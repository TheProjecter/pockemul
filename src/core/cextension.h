#ifndef _CEXTENSION_H
#define _CEXTENSION_H

#include <QString>
#include <QAction>
#include <QMenu>

//PC-1500 ext
#define ID_CE151	0
#define ID_CE153	1
#define ID_CE155	2
#define ID_CE156	3
#define ID_CE157	4
#define ID_CE158	5
#define ID_CE159	6
#define ID_CE160	7
#define ID_CE161	8       // 16 Kb module

// PC-13xx PC-14XX ext
#define ID_CE201M	9
#define ID_CE202M	10
#define ID_CE203M	11
#define ID_CE210M	12
#define ID_CE211M	13
#define ID_CE212M	14
#define ID_CE2H16M	15
#define ID_CE2H32M	16
#define ID_CE2H64M	17

// PC-1600 ext
#define ID_CE1600M  18      // 32 Kb module
#define ID_CE1601M  19      // 64 Kb module
#define ID_CE1620M  20      // 32 Kb eprom
#define ID_CE1625M  21      // 32 Kb eprom
// Custom PC-1600 module
#define ID_CE16096  22      // 96 Kb module
#define ID_CE16128  23      // 128 Kb module
#define ID_CE16160  24      // 160 Kb module
#define ID_CE16192  25      // 192 Kb module
#define ID_CE16224  26      // 224 Kb module
#define ID_CE16256  27      // 256 Kb module





class CExtension{
public:
    const char*	GetClassName(){ return("CExtension"); };

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
    const char*	GetClassName(){ return("CExtensionArray"); };

    QString Id;
    QString Description;
    QMenu	*Menu;
    QAction *emptyAction;
    QAction *loadAction;
    QAction *saveAction;

    QActionGroup *actionGroup;

    CExtension	*ExtArray[30];
//    QList<CExtension> ExtList;

    void setAvailable(int ind,bool value);
    void setAvailable(int ind,bool value,bool checked);
    void setChecked(int ind,bool value);


    CExtensionArray(QString,QString);               //[constructor]
    ~CExtensionArray(void)							//[constructor]
    {
        delete Menu;
        delete emptyAction;
        delete loadAction;
        delete saveAction;
        delete actionGroup;
        for (int i=0;i<30;i++) delete ExtArray[i];

    }
};



#endif
