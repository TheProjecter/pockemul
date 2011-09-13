#include "dialogdasm.h"
#include "ui_dialogdasm.h"
#include "pcxxxx.h"
#include "Debug.h"
#include "bineditor/bineditor.h"
#include "ui/cregssc61860widget.h"


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
    memHexEditor = new BINEditor::BinEditor(ui->memframe);
//    imemHexEditor->setFocus();

    connect(ui->refreshButton,SIGNAL(clicked()),this,SLOT(loadImem()));
    connect(ui->pbStart,SIGNAL(clicked()),this,SLOT(start()));
    connect(ui->pbStop,SIGNAL(clicked()),this,SLOT(stop()));
    connect(ui->pbStep,SIGNAL(clicked()),this,SLOT(step()));
    connect(pPC,SIGNAL(RefreshDasm()),this,SLOT(RefreshDasm()));

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    ui->codelistWidget->setFont(font);

    Cregssc61860Widget *reg = new Cregssc61860Widget();

    resize(this->size());
}

DialogDasm::~DialogDasm()
{
    delete ui;
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
            ui->codelistWidget->setCurrentRow(i);
            ui->codelistWidget->currentItem()->setSelected(true);
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
                    ui->codelistWidget->setCurrentRow(ui->codelistWidget->count()-1);
                    ui->codelistWidget->currentItem()->setSelected(true);
                    //ListBox_SetItemData(g_hWndListDasm, Index, 0);
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
            ui->codelistWidget->setCurrentRow(ui->codelistWidget->count()-1);
            ui->codelistWidget->currentItem()->setSelected(true);
            //ui->codelistWidget->currentItem()->setFont();

            //ListBox_SetItemData(g_hWndListDasm, Index, pPC->pCPU->pDEBUG->DasmAdr);

            //ListBox_SetCurSel(g_hWndListDasm, Index);
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
//                ListBox_AddItemData(g_hWndListDasm,pPC->pCPU->pDEBUG->Buffer);
//                ListBox_SetItemData(g_hWndListDasm, Index, pPC->pCPU->pDEBUG->DasmAdr);
                Index++;
            }
        }
        pPC->pCPU->Regs_Info(2);


        //ShowReg();
    }

}

void DialogDasm::loadImem()
{
    if (pPC) {
        imemHexEditor->setData(pPC->pCPU->getimem());
    }
    update();
}

void DialogDasm::loadMem()
{
    //if (pPC) memHexEditor->setData(ba);
}

void DialogDasm::resizeEvent( QResizeEvent * event )
{
    imemHexEditor->resize( ui->imemframe->size() );
}

void DialogDasm::start()
{
    pPC->DasmFlag = false;
    pPC->pCPU->halt = 0;
    pPC->DasmStep = false;
    //Edit_GetText(g_hWndBreakDasm,Buffer,5);
    //sscanf(Buffer,"%04x",&g_BreakPointAdr);
    ui->codelistWidget->clear();
    Index = 0;
    if (pPC->BreakSubLevel == pPC->pCPU->CallSubLevel)
        pPC->BreakSubLevel = -1;

}

void DialogDasm::stop()
{
    pPC->DasmFlag = false;
    pPC->pCPU->halt = 1;
}

void DialogDasm::step()
{
    pPC->DasmFlag = true;
    pPC->pCPU->halt = 0;
    pPC->DasmStep = true;
}
