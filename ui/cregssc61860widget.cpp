#include "cregssc61860widget.h"
#include "ui_cregssc61860widget.h"

Cregssc61860Widget::Cregssc61860Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Cregssc61860Widget)
{
    ui->setupUi(this);
}

Cregssc61860Widget::~Cregssc61860Widget()
{
    delete ui;
}
