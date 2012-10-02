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
    int result = EMPTY;

    if (ItemText == "PC 1245")	result = PC1245;
    if (ItemText == "MC 2200")	result = MC2200;
    if (ItemText == "PC 1250")	result = PC1250;
    if (ItemText == "PC 1251")	result = PC1251;
    if (ItemText == "PC 1255")	result = PC1255;
    if (ItemText == "Tandy PC-3")	result = TandyPC3;
    if (ItemText == "Tandy PC-3 (4Ko)")	result = TandyPC3EXT;
    if (ItemText == "PC 1260")	result = PC1260;
    if (ItemText == "PC 1261")	result = PC1261;
    if (ItemText == "PC 1262")	result = PC1262;
    if (ItemText == "PC 1280")	result = PC1280;
    if (ItemText == "PC 1350")	result = PC1350;
    if (ItemText == "PC 1360")	result = PC1360;
    if (ItemText == "PC 1401")	result = PC1401;
    if (ItemText == "PC 1402")	result = PC1402;
    if (ItemText == "PC 1403")	result = PC1403;
    if (ItemText == "PC 1421")	result = PC1421;
    if (ItemText == "PC 1403H")	result = PC1403H;
    if (ItemText == "PC 1450")	result = PC1450;
    if (ItemText == "PC 1475")	result = PC1475;
    if (ItemText == "PC 1500")	result = PC1500;
    if (ItemText == "PC 1500A")	result = PC1500A;
    if (ItemText == "Tandy PC-2")	result = TandyPC2;
    if (ItemText == "PC 1251H")	result = PC1251H;
    if (ItemText == "PC 1600")	result = PC1600;
    if (ItemText == "PC 2500")	result = PC2500;

    if (ItemText == "CE-125")	result = CE125;
    if (ItemText == "MP-220")	result = MP220;
    if (ItemText == "CE-120P")	result = CE120P;
    if (ItemText == "CE-126P")	result = CE126P;
    if (ItemText == "CE-123P")	result = CE123P;
    if (ItemText == "CE-129P")	result = CE129P;
    if (ItemText == "CE-140P")	result = CE140P;
    if (ItemText == "CE-140F")	result = CE140F;
    if (ItemText == "CE-150")	result = CE150;
    if (ItemText == "CE-152")	result = CE152;
    if (ItemText == "26-3591")	result = TANDY263591;
		
    if (ItemText == "Serial Console")	result = SerialConsole;
    if (ItemText == "11Pins Cable")	result = CABLE11Pins;
    if (ItemText == "Potar")	result = POTAR;
    if (ItemText == "Simulator")	result = Simulator;

    if (ItemText == "CE-1600P")	result = CE1600P;
    if (ItemText == "Canon X-07") result = X07;
    if (ItemText == "Canon X-710") result = X710;
    if (ItemText == "PC-E500") result = E500;
    if (ItemText == "PC-E550") result = E550;
    if (ItemText == "PC-G850V") result = G850V;
    if (ItemText == "Casio PB-1000") result = PB1000;
    if (ItemText == "Casio PB-2000C") result = PB2000;
    if (ItemText == "Casio MD-100") result = MD100;
    if (ItemText == "Casio FP-100") result = FP100;
    if (ItemText == "Casio FP-200") result = FP200;
    if (ItemText == "Casio Z-1GR") result = Z1GR;
    if (ItemText == "Casio Z-1") result = Z1;

    if (result != EMPTY)	{
		this->done(result);
	}
//	else
//		QMessageBox::about(this, tr("Attention"),"Please choose a pocket model or Cancel");
}
//
void DialogStartup::on_cancelButton_clicked()
{
}
//
