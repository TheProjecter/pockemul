#include "dialogvkeyboard.h"
#include "ui_dialogvkeyboard.h"


#include "mainwindowpockemul.h"
#include "pobject.h"
#include "Keyb.h"
#include "Inter.h"

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
    connect(ui->SendPB,SIGNAL(clicked()),this,SLOT(senData()));
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

void DialogVKeyboard::senData()
{

    int charWait = ui->charWaitLE->text().toInt();
    int crWait = ui->CRWaitLE->text().toInt();

    QByteArray ba = ui->textEdit->toPlainText().toAscii();

    for (int i=0;i<ba.size();i++) {
        int c = ba.at(i);
        if (c==0x0a) c = Qt::Key_Return;
        QKeyEvent *event = new QKeyEvent ( QEvent::KeyPress, c, Qt::NoModifier);
        QCoreApplication::postEvent (pPC, event);
        qint64 refstate = pPC->pTIMER->state;
        do{QCoreApplication::processEvents(QEventLoop::AllEvents, 100);} while (pPC->pTIMER->msElapsed(refstate)<charWait);

//        delete(event);
        event = new QKeyEvent ( QEvent::KeyRelease, c, Qt::NoModifier);
        QCoreApplication::postEvent (pPC, event);
        refstate = pPC->pTIMER->state;
        do{QCoreApplication::processEvents(QEventLoop::AllEvents, (c==Qt::Key_Return)?crWait:charWait);} while (pPC->pTIMER->msElapsed(refstate)<100);
    }
}


