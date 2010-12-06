#ifndef DIALOGDUMP_H
#define DIALOGDUMP_H
//
#include "ui_dialogdump.h"
//#include "qhexedit.h"

class HexViewer;
class CpcXXXX;
//
class DialogDump : public QDialog, public Ui::DialogDump
{
Q_OBJECT
public:
	DialogDump( QWidget * parent = 0, Qt::WFlags f = 0 );


	HexViewer *hexeditor;
    //QHexEdit *hextemp;

private slots:
	void slotDump( QTableWidgetItem * , QTableWidgetItem * );
	void LoadBin( void );
	void SaveBin( void );

protected:
	void resizeEvent(QResizeEvent *);

private:
//	HexWidget *hexeditor;
	void LoadSlot(void);
	CpcXXXX *pPC;	
};
#endif





