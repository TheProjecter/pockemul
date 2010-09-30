#include <QTextCursor>
#include <QCloseEvent>

#include "dialogconsole.h"
#include "common.h"
#include "pcxxxx.h"
#include "sio.h"

//
DialogConsole::DialogConsole( QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);
	setFocusPolicy(Qt::StrongFocus);

    connect(pbSend, SIGNAL(clicked()), this, SLOT(sendData())); 
	pSIO = (Csio *) parent;
	
}
//
void DialogConsole::refresh( void)
{
	static int currentIndex = 0;
	
	if (! pSIO) return;
    if (pSIO->pTIMER) ConnectLbl->setText("Connected to : " + pSIO->pTIMER->pPC->getName());
	if (pSIO->baOutput.size() > currentIndex)
	{
		int len = pSIO->baOutput.size()-currentIndex;
		textEdit_out->textCursor().setPosition(currentIndex);
		
		for (int i=0 ; i < len ; i++)
		{
			QString add;
			const unsigned char loc = pSIO->baOutput.at(currentIndex + i);
			if ((loc < 0x20) || (loc > 0x7F)) add = QString(" [0x%1] ").arg(loc,2,16,QChar('0'));
				else add = QString(loc);
			if ( cbCRLF->isChecked() && (loc == 0x0D) ) add = QString(loc); // TO DO : Manage CR
			textEdit_out->insertPlainText(add);
		}
		textEdit_out->textCursor().setPosition(currentIndex);

//	SCROOL TO BOTTOM
		QTextCursor cursor(textEdit_out->textCursor());
		cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		textEdit_out->setTextCursor(cursor);
		
		currentIndex += len;
	}
}

void DialogConsole::sendData( void)
{
	
	if (! pSIO) return;
	
	pSIO->clearInput();
	pSIO->baInput.append(textEdit_in->toPlainText () );
	pSIO->baInput.append(0x1A);
	pSIO->startTransfer();
}

void DialogConsole::closeEvent(QCloseEvent *event)
{
	pSIO->ToDestroy = true;
	event->accept();
}

