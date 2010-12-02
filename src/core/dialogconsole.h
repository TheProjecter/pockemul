#ifndef DIALOGCONSOLE_H
#define DIALOGCONSOLE_H
//
#include "ui_serialconsole.h"

class Csio;
//
class DialogConsole : public QDialog, public Ui::DialogConsole
{
Q_OBJECT
public:
	DialogConsole( QWidget * parent = 0, Qt::WFlags f = 0 );
	
	void refresh( void);

private slots:
	void	sendData( void);
    void    changeBaudrate(QString);

protected:
	void closeEvent(QCloseEvent *event);
	Csio *pSIO;
};
#endif
