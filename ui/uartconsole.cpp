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
