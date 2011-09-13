#include "cregssc61860widget.h"
#include "ui_cregssc61860widget.h"
#include "ui/cregcpu.h"
#include "cpu.h"

Cregssc61860Widget::Cregssc61860Widget(QWidget *parent,CCPU *pCPU) :
    CregCPU(parent,pCPU),
    ui(new Ui::Cregssc61860Widget)
{
    ui->setupUi(this);
}

Cregssc61860Widget::~Cregssc61860Widget()
{
    delete ui;
}

void Cregssc61860Widget::refresh()
{
    if (!pCPU) return;

    ui->reg00->setText(QString("%1").arg(pCPU->imem[0],2,16,QChar('0')));
    ui->reg01->setText(QString("%1").arg(pCPU->imem[1],2,16,QChar('0')));
    ui->reg02->setText(QString("%1").arg(pCPU->imem[2],2,16,QChar('0')));
    ui->reg03->setText(QString("%1").arg(pCPU->imem[3],2,16,QChar('0')));
    ui->reg04->setText(QString("%1").arg(pCPU->imem[4],2,16,QChar('0')));
    ui->reg05->setText(QString("%1").arg(pCPU->imem[5],2,16,QChar('0')));
    ui->reg06->setText(QString("%1").arg(pCPU->imem[6],2,16,QChar('0')));

    update();
}
