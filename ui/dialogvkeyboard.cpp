#include "dialogvkeyboard.h"
#include "ui_dialogvkeyboard.h"


#include "mainwindowpockemul.h"
#include "pobject.h"
#include "Keyb.h"

extern MainWindowPockemul *mainwindow;
extern QList<CPObject *> listpPObject;

DialogVKeyboard::DialogVKeyboard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVKeyboard)
{
    ui->setupUi(this);

    pPC = (CPObject*) parent;

    setWindowTitle("Keyboard Simulator for "+pPC->getName());
    PopulateKeyList();

    connect(ui->keylistWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(InsertKeySlot(QListWidgetItem*)));
}

DialogVKeyboard::~DialogVKeyboard()
{
    delete ui;
}

void DialogVKeyboard::PopulateKeyList()
{
    QListWidgetItem *item;

    QList<CKey>::iterator it;
    for (it = pPC->pKEYB->Keys.begin(); it != pPC->pKEYB->Keys.end(); ++it)
    {
        item = new QListWidgetItem(it->Description, ui->keylistWidget);
        item->setData( Qt::UserRole, qVariantFromValue( it->ScanCode ) );
    }
}

void DialogVKeyboard::InsertKeySlot(QListWidgetItem *item)
{
    ui->textEdit->textCursor().insertText("["+item->text()+"]");
}


