#include "uartconsole.h"
#include "ui_uartconsole.h"

CUartConsole::CUartConsole(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CUartConsole)
{
    ui->setupUi(this);
}

CUartConsole::~CUartConsole()
{
    delete ui;
}

void CUartConsole::newOutputByte(qint8 data) {
    if (data != 0x0D) {
        QString buf = ui->textEdit_out->toPlainText();
        ui->textEdit_out->setPlainText(buf + QString(data));
    }
}
