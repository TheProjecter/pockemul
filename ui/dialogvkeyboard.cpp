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

    dict.clear();
    QList<CKey>::iterator it;
    for (int i=0; i< pPC->pKEYB->Keys.size();i++) {
        CKey *k = &(pPC->pKEYB->Keys[i]);
        item = new QListWidgetItem(k->Description, ui->keylistWidget);
        item->setData( Qt::UserRole, qVariantFromValue( k->ScanCode ) );
//        dict.insert(it->Description,it->ScanCode);
        dict.insert(k->Description,k);
    }
//    for (it = pPC->pKEYB->Keys.begin(); it != pPC->pKEYB->Keys.end(); ++it)
//    {
//        item = new QListWidgetItem(it->Description, ui->keylistWidget);
//        item->setData( Qt::UserRole, qVariantFromValue( it->ScanCode ) );
////        dict.insert(it->Description,it->ScanCode);
//        dict.insert(it);
//    }

}

void DialogVKeyboard::InsertKeySlot(QListWidgetItem *item)
{
    ui->textEdit->textCursor().insertText("["+item->text()+"]");
}

void DialogVKeyboard::processEscKey(QString word) {

    CKey *k = dict.value(word);
    if (k->MasterScanCode==0x00) final.append(k->ScanCode);
    else {
        if (k->Modifier == "shift") {
//            final.append(K_SHIFT_DOWN_MOD);
            final.append(k->MasterScanCode | 0x2000000);
//            final.append(K_SHIFT_UP_MOD);
        }
        if (k->Modifier == "ctrl") {
//            final.append(K_CTRL_DOWN_MOD);
            final.append(k->MasterScanCode | 0x4000000);
//            final.append(K_CTRL_UP_MOD);
        }
    }

}

void DialogVKeyboard::senData()
{

    int charWait = ui->charWaitLE->text().toInt();
    int crWait = ui->CRWaitLE->text().toInt();

    QString finalString = ui->textEdit->toPlainText();
    // replace special keys
    // replace 0x0D,0x0A en 0x0D
    finalString.replace(QChar('\n'),QChar('\r'));

    QStringList list = finalString.split(QRegExp("[\\[\\]]"));

    final.clear();
    for (int i=0; i < list.size(); ++i)
    {
        QString word = list[i];
        if (dict.contains(word))
        {
            processEscKey(word);
        }
        else {
            QByteArray b = word.toAscii();
            for (int j=0;j<b.size();j++) {
                QString word (b.at(j));
                if (dict.contains(word))
                    processEscKey(word);
                else
                    final.append(b.at(j));
            }
        }
    }

    for (int i=0;i<final.size();i++) {
        int b = final.at(i);

        int c = convertKeyCode(b);
        QKeyEvent *event = new QKeyEvent ( QEvent::KeyPress, c, Qt::NoModifier);
        QCoreApplication::postEvent (pPC, event);
        qint64 refstate = pPC->pTIMER->state;
        do{QCoreApplication::processEvents(QEventLoop::AllEvents, 100);} while (pPC->pTIMER->msElapsed(refstate)<charWait);

        event = new QKeyEvent ( QEvent::KeyRelease, c, Qt::NoModifier);
        QCoreApplication::postEvent (pPC, event);
        refstate = pPC->pTIMER->state;
        do{QCoreApplication::processEvents(QEventLoop::AllEvents, (c==Qt::Key_Return)?crWait:charWait);} while (pPC->pTIMER->msElapsed(refstate)<100);
    }
}

int DialogVKeyboard::convertKeyCode(int c) {
    int key = 0;
    switch (c) {
    case K_SHT:	key = Qt::Key_Shift; break;
    case K_CTRL:key = 	Qt::Key_Control; break;
    case K_RET:	key = Qt::Key_Return; break;
    case K_DEL:	key = Qt::Key_Delete; break;
    case K_INS:	key = Qt::Key_Insert; break;
    case K_QUOTE:key = Qt::Key_QuoteLeft; break;
    case K_TAB: key =   Qt::Key_Tab; break;
    case ' ':	key = 	Qt::Key_Space; break;
    case '.':	key = 	Qt::Key_Period; break;
    case '+':	key = 	Qt::Key_Plus; break;
    case '-':	key = 	Qt::Key_Minus; break;
    case '*':	key = 	Qt::Key_Asterisk; break;
    case '/':	key = 	Qt::Key_Slash; break;
    case '=':	key = 	Qt::Key_Equal; break;
    case '"':	key = 	Qt::Key_QuoteDbl; break;
    case ';':	key = 	Qt::Key_Semicolon; break;
    case ',':	key = 	Qt::Key_Comma; break;
    case '(':   key =   Qt::Key_ParenLeft; break;
    case ')':   key =   Qt::Key_ParenRight; break;
    case K_BS:	key = Qt::Key_Backspace; break;
    case '$':	key = 	Qt::Key_Dollar; break;
    case '&':	key = 	Qt::Key_Ampersand; break;
    case K_SML:	key = Qt::Key_CapsLock; break;
    case K_LA:	key = Qt::Key_Left; break;
    case K_RA:	key = Qt::Key_Right; break;
    case K_UA:	key = Qt::Key_Up; break;
    case K_DA:	key = Qt::Key_Down; break;
    case K_F1:	key = Qt::Key_F1; break;
    case K_F2:	key = Qt::Key_F2; break;
    case K_F3:	key = Qt::Key_F3; break;
    case K_F4:	key = Qt::Key_F4; break;
    case K_F5:	key = Qt::Key_F5; break;
    case K_F6:	key = Qt::Key_F6; break;
    case K_F7:	key = Qt::Key_F7; break;
    case K_CLR:	key = Qt::Key_F8; break;
    case K_DEF:	key = Qt::Key_F9; break;
    case K_BRK:	key = Qt::Key_F11; break;
    default: key = c;
    }
    return key;
}
