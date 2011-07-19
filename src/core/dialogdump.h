#ifndef DIALOGDUMP_H
#define DIALOGDUMP_H
//
#include "ui_dialogdump.h"
#include "qhexedit.h"
#include "bineditor/bineditor.h"

class HexViewer;
class CpcXXXX;
//
class DialogDump : public QDialog, public Ui::DialogDump
{
Q_OBJECT
public:
	DialogDump( QWidget * parent = 0, Qt::WFlags f = 0 );


    BINEditor::BinEditor *hexeditor;
    QHexEdit *hextemp;

private slots:
	void slotDump( QTableWidgetItem * , QTableWidgetItem * );
	void LoadBin( void );
	void SaveBin( void );
    void Find( void );
    void FindNext( void );
    void FindPrevious(void);
    void JumpTo( void);

protected:
	void resizeEvent(QResizeEvent *);

private:
//	HexWidget *hexeditor;
	void LoadSlot(void);
	CpcXXXX *pPC;	
    int findpos;
};
#endif





