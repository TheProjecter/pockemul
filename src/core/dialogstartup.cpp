#include <QtGui>

#include "dialogstartup.h"
#include "init.h"

//
DialogStartup::DialogStartup( QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);
	
}
//
void DialogStartup::on_listWidget_itemDoubleClicked()
{
	QString ItemText = listWidget->currentItem()->text();
	create(ItemText);
}

void DialogStartup::on_listWidget_2_itemDoubleClicked()
{
	QString ItemText = listWidget_2->currentItem()->text();
	create(ItemText);
}

void DialogStartup::on_listWidget_3_itemDoubleClicked()
{
	QString ItemText = listWidget_3->currentItem()->text();
	create(ItemText);
}

void DialogStartup::create(QString ItemText)
{
	int result = 0;

	if (ItemText == "PC 1245")	result = 10;
	if (ItemText == "PC 1250")	result = 11;
	if (ItemText == "PC 1251")	result = 12;
	if (ItemText == "PC 1255")	result = 13;
	if (ItemText == "Tandy PC-3")	result = 14;
	if (ItemText == "PC 1260")	result = 15;
	if (ItemText == "PC 1261")	result = 16;
	if (ItemText == "PC 1262")	result = 17;
	if (ItemText == "PC 1350")	result = 18;
	if (ItemText == "PC 1360")	result = 19;
	if (ItemText == "PC 1401")	result = 20;
	if (ItemText == "PC 1402")	result = 21;
	if (ItemText == "PC 1403")	result = 22;
	if (ItemText == "PC 1403H")	result = 23;
	if (ItemText == "PC 1450")	result = 24;
	if (ItemText == "PC 1475")	result = 25;
	if (ItemText == "PC 1500")	result = 30;
	if (ItemText == "PC 1500A")	result = 31;	
	if (ItemText == "Tandy PC-2")	result = 32;	
	if (ItemText == "PC 1251H")	result = 33;
    if (ItemText == "PC 1600")	result = 34;

	if (ItemText == "CE-125")	result = 101;	
	if (ItemText == "CE-126P")	result = 102;	
	if (ItemText == "CE-123P")	result = 103;	
	if (ItemText == "CE-129P")	result = 104;	
	if (ItemText == "CE-150")	result = 107;	
	if (ItemText == "CE-152")	result = 108;	
	if (ItemText == "26-3591")	result = 109;	
		
	if (ItemText == "Serial Console")	result = 105;	
    if (ItemText == "11Pins Cable")	result = 106;
    if (ItemText == "Potar")	result = 111;

    if (ItemText == "CE-1600P")	result = 110;
		
	if (result)	{
		this->done(result);
	}
	else
		QMessageBox::about(this, tr("Attention"),"Please choose a pocket model or Cancel");
}
//
void DialogStartup::on_cancelButton_clicked()
{
}
//
