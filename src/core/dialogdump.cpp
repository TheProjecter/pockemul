#include <QFileDialog>

#include "dialogdump.h"
#include "hexviewer.h"
//#include "qhexedit.h"
#include "common.h"
#include "pcxxxx.h"
#include "slot.h"
#include "Log.h"

#include "bineditor/bineditor.h"

//

DialogDump::DialogDump( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
	
	pPC =  (CpcXXXX *) parent;
	lbl_connected->setText(tr("Connected to : %1").arg(pPC->getName()));

    connect(twSlot, SIGNAL(currentItemChanged ( QTableWidgetItem * , QTableWidgetItem * )), this, SLOT(slotDump( QTableWidgetItem * , QTableWidgetItem * )));
    connect(pbLoadBin, SIGNAL(clicked()), this, SLOT(LoadBin())); 
    connect(pbSaveBin, SIGNAL(clicked()), this, SLOT(SaveBin())); 
    connect(leFind,SIGNAL(returnPressed()),this,SLOT(Find()));
    connect(pbFindPrevious,SIGNAL(clicked()),this,SLOT(FindPrevious()));
    connect(pbFindNext,SIGNAL(clicked()),this,SLOT(FindNext()));
    connect(leJump,SIGNAL(returnPressed()),this,SLOT(JumpTo()));

#if 1
    hexeditor = new BINEditor::BinEditor(framedump);
    hexeditor->setFocus();
#else
    hextemp = new QHexEdit(framedump);
    hextemp->setFocus();
#endif
      LoadSlot();


	resize( 605,400);
}

void DialogDump::Find(void) {
    QString search = leFind->text();
    hexeditor->highlightSearchResults(search.toAscii());
    findpos = hexeditor->find(search.toAscii(),hexeditor->cursorPosition());
}
void DialogDump::JumpTo(void) {
    bool ok;
    hexeditor->jumpToAddress(leJump->text().toLongLong(&ok,16));
}
void DialogDump::FindPrevious(void) {
    findpos = hexeditor->find(leFind->text().toAscii(),findpos-1,QTextDocument::FindBackward);
}
void DialogDump::FindNext(void) {
    //MSG_ERROR("ok");
    findpos = hexeditor->find(leFind->text().toAscii(),findpos+1);
    //MSG_ERROR(tr("ok").arg(findpos));
}

void DialogDump::LoadBin(void)
{
	// Launch Open File Dialog
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 ".",
                                                 tr("Binary File (*.bin)"));

	if( fileName.isNull() ) 
		return ;
		
	QFile file(fileName);
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);
	
	// Ask confirmation
	bool ok;
	int adr = lineEditAdr->text().toInt(&ok, 16);
	if (QMessageBox::warning(this, tr("PockEmul"),
	                                 tr("Load binay file : %1\n"
	                                 "At adr (hex): %2\n "
	                                 "Do you confirm ?\n").arg(fileName).arg(adr,5,16,QChar('0')),
	                                 "Yes",
	                                 "Abort", 0, 0, 1) == 1)	return;

	// Load the file
	in.readRawData ( (char *) &(pPC->mem[adr]), file.size() );
}

void DialogDump::SaveBin(void)
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                            ".",
                            tr("Binary File (*.bin)"));

	if( fileName.isNull() ) 
		return ;
	QFile file(fileName);
 	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);

	bool ok;
	int adrFrom = lineEditAdrFrom->text().toInt(&ok, 16);
	int adrTo   = lineEditAdrTo->text().toInt(&ok, 16);

	// Save to file
	out.writeRawData ( (char *) &(pPC->mem[adrFrom]), adrTo - adrFrom +1 );
}

void DialogDump::LoadSlot(void)
{
	QTableWidgetItem *newItem ;
	QList<CSlot>::iterator it;
	QString str;
			
	twSlot->setRowCount(pPC->SlotList.size()); 
	int row = 0;

	if (pPC->SlotList.size())
	{
		for (it = pPC->SlotList.begin(); it != pPC->SlotList.end(); ++it)
		{
			int col = 0;
	
			newItem = new QTableWidgetItem(it->getLabel());
			twSlot->setItem(row, col, newItem);
			AddLog(LOG_MASTER,tr("Insert item (%1,%2)=%3").arg(row).arg(col).arg(it->getLabel()));	
			col++;
			newItem = new QTableWidgetItem(str.setNum(it->getSize()));
			twSlot->setItem(row, col, newItem);
			col++;
			newItem = new QTableWidgetItem(tr("0x%1").arg(it->getAdr(),5,16,QChar('0')));
			twSlot->setItem(row, col, newItem);
			col++;
			newItem = new QTableWidgetItem(tr("0x%1").arg(it->getAdr()+it->getSize()*1024 - 1,5,16,QChar('0')));
			twSlot->setItem(row, col, newItem);
			col++;
	
			row++;	
		}
	}

		
	twSlot->resizeColumnsToContents();
	twSlot->update();
}

void DialogDump::resizeEvent( QResizeEvent * event )
{
#if 1
    hexeditor->resize( framedump->size() );
#else
    hextemp->resize(framedump->size());
#endif
}

void DialogDump::slotDump( QTableWidgetItem * current, QTableWidgetItem * previous)
{
	bool ok;
	
    // Check if dataChanged , ask then save (not in ROM !!!)
    // Modify ROM ??? save ROM to file ... have to think about this

	int adr = twSlot->item(twSlot->currentRow(),2)->text().toInt(&ok,16);
	int size = twSlot->item(twSlot->currentRow(),1)->text().toInt() * 1024;

#if 1
    QByteArray *ba= new QByteArray((const char*)&(pPC->mem[adr]),size);
    hexeditor->data().clear();
    hexeditor->setReadOnly(true);
    hexeditor->setData(*ba);

#else
    QByteArray *ba= new QByteArray((const char*)&(pPC->mem[adr]),size);
    hextemp->data().clear();
    hextemp->setAddressOffset(adr);
    hextemp->setData(*ba);//data().append(&(pPC->mem[adr]),size);

    hextemp->setFocus();
#endif
	update();

}
