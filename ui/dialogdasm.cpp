//TODO Contextual menu on breakpoints


#include "dialogdasm.h"
#include "ui_dialogdasm.h"
#include "pcxxxx.h"
#include "Debug.h"
#include "bineditor/bineditor.h"
#include "ui/cregssc61860widget.h"
#include "cpu.h"
#include "cregcpu.h"

DialogDasm::DialogDasm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDasm)
{
    ui->setupUi(this);

    pPC = (CpcXXXX*)parent;
    Index = 0;
    MaxAdr = 0;
    NextMaxAdr = 0;

    imemHexEditor = new BINEditor::BinEditor(ui->imemframe);
//    memHexEditor = new BINEditor::BinEditor(ui->memframe);

    connect(ui->tbStart,SIGNAL(clicked()),this,SLOT(start()));
//    connect(ui->pbStop,SIGNAL(clicked()),this,SLOT(stop()));
    connect(ui->tbStep,SIGNAL(clicked()),this,SLOT(step()));
    connect(ui->tbStepOver,SIGNAL(clicked()),this,SLOT(stepOver()));
    connect(pPC,SIGNAL(RefreshDasm()),this,SLOT(RefreshDasm()));
    connect(ui->tbAddBrkPt,SIGNAL(clicked()),this,SLOT(addBreakPoint()));
    connect(ui->lwBreakPts,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(breakPointChanged(QListWidgetItem*)));

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    ui->codelistWidget->setFont(font);

    if (pPC->pCPU->regwidget) {
        regwidget = pPC->pCPU->regwidget;
        regwidget->setParent(ui->regframe);

    }
    this->show();
    this->resize(545,490);
    regwidget->show();

    imem=true;

}

DialogDasm::~DialogDasm()
{
    delete ui;
}

void DialogDasm::selectRow(int index) {

    QListWidgetItem *item = ui->codelistWidget->currentItem();
    if (item) {
        ui->codelistWidget->currentItem()->setBackground(Qt::white);
        ui->codelistWidget->currentItem()->setForeground(Qt::black);
    }
    ui->codelistWidget->setCurrentRow(index);
    //ui->codelistWidget->currentItem()->setSelected(true);
    ui->codelistWidget->currentItem()->setBackground(Qt::blue);
    ui->codelistWidget->currentItem()->setForeground(Qt::red);
    //ui->codelistWidget->currentItem()->font().setBold(true);
//    font.setBold(true);
//    ui->codelistWidget->currentItem()->setFont(font);
}

bool DialogDasm::IsAdrInList(qint32 adr)
{

    int NbLig;

    NbLig = ui->codelistWidget->count();

    for (int i=0;i<NbLig;i++)
    {
        QListWidgetItem *item = ui->codelistWidget->item(i);
        if (adr == item->data(Qt::UserRole))
        {
            selectRow(i);

            return(true);
        }
    }

    return(false);

}

void DialogDasm::RefreshDasm()
{

    if ( pPC->pCPU->pDEBUG->debugged)
    {
        QString	text;

        if (! IsAdrInList(pPC->pCPU->pDEBUG->DasmAdr))
        {
            if (MaxAdr > pPC->pCPU->pDEBUG->DasmAdr)
            {
                // effacer tout et recommencer au debut
                ui->codelistWidget->clear();

                Index		= 0;
                MaxAdr		= pPC->pCPU->pDEBUG->DasmAdr;
                NextMaxAdr	= pPC->pCPU->pDEBUG->NextDasmAdr;
            }
            else
            {
                if (pPC->pCPU->pDEBUG->DasmAdr > NextMaxAdr)
                {
                    // Insert a separator
                    text = QString("%1:---------------------").arg(pPC->pCPU->pDEBUG->DasmAdr-1,5,16,QChar('0'));
                    QListWidgetItem *item = new QListWidgetItem(text);
                    int adr = pPC->pCPU->pDEBUG->DasmAdr;
                    item->setData(Qt::UserRole,QVariant(adr));
                    ui->codelistWidget->addItem(item);
                    selectRow(ui->codelistWidget->count()-1);

                    NextMaxAdr = pPC->pCPU->pDEBUG->NextDasmAdr;
                    Index++;
                }
                MaxAdr		= pPC->pCPU->pDEBUG->DasmAdr;
                NextMaxAdr	= pPC->pCPU->pDEBUG->NextDasmAdr;
            }

            QListWidgetItem *item = new QListWidgetItem(pPC->pCPU->pDEBUG->Buffer);
            int adr = pPC->pCPU->pDEBUG->DasmAdr;
            item->setData(Qt::UserRole,adr);
            ui->codelistWidget->addItem(item);
            selectRow(ui->codelistWidget->count()-1);
            Index++;
// full until 15 lines
            for (int j=Index;j<15;j++)
            {
                pPC->pCPU->pDEBUG->DisAsm_1(NextMaxAdr);
                MaxAdr		= pPC->pCPU->pDEBUG->DasmAdr;
                NextMaxAdr	= pPC->pCPU->pDEBUG->NextDasmAdr;
                QListWidgetItem *item = new QListWidgetItem(pPC->pCPU->pDEBUG->Buffer);
                int adr = pPC->pCPU->pDEBUG->DasmAdr;
                item->setData(Qt::UserRole,adr);

                ui->codelistWidget->addItem(item);
                Index++;
            }
        }

    }
    if (regwidget) regwidget->refresh();
    loadImem();
    loadMem();

}

void DialogDasm::loadImem()
{
    if (pPC) {
        if (imem)
            imemHexEditor->setData(pPC->pCPU->getimem());
        else
            imemHexEditor->setData(pPC->getmem());
    }
    update();
}

void DialogDasm::loadMem()
{
//    if (pPC) {
//        memHexEditor->setData(pPC->getmem());
//    }
//    update();
}

void DialogDasm::resizeEvent( QResizeEvent * event )
{
    imemHexEditor->resize( ui->imemframe->size());
//    memHexEditor->resize(ui->memframe->size());
    if (regwidget) regwidget->resize(ui->regframe->size());
}

void DialogDasm::start()
{
    pPC->DasmFlag = false;
    pPC->DasmLastAdr = 0xFFFFFFFF;
    ui->codelistWidget->clear();
    Index = 0;
    if (pPC->BreakSubLevel == pPC->pCPU->CallSubLevel)
        pPC->BreakSubLevel = -1;

}

void DialogDasm::stop()
{
//    pPC->DasmFlag = false;
//    pPC->pCPU->halt = 1;
    pPC->DasmStep = true;
}

void DialogDasm::step()
{
    pPC->BreakSubLevel = 99999;
    pPC->DasmStep = true;
    pPC->DasmFlag = false;
}

void DialogDasm::stepOver()
{
    pPC->BreakSubLevel = pPC->pCPU->CallSubLevel;
    pPC->DasmStep = true;
    pPC->DasmFlag = false;
}

quint16 DialogDasm::getValue()
{
    return ui->lineEdit->text().toLong(0,16);
}

void DialogDasm::addBreakPoint()
{
    pPC->BreakPoints[ui->breakPointLineEdit->text().toLongLong(0,16)] = Qt::Checked;

    QListWidgetItem *_item = new QListWidgetItem(ui->breakPointLineEdit->text());
    _item->setCheckState(Qt::Checked);
    ui->lwBreakPts->addItem(_item);
}

void DialogDasm::breakPointChanged(QListWidgetItem *item)
{
    pPC->BreakPoints[item->text().toLongLong(0,16)] = item->checkState();

}
