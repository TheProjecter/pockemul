#include "ccompletion.h"
#include "qcalltip.h"
#include "qeditor.h"
#include "qdocumentline.h"
#include "qcodecompletionwidget.h"
#include <QTime>
#include <QAction>

//#include "mainwindowpockemul.h"
#include "ui/windowide.h"

//extern MainWindowPockemul *mainwindow;

CCompletion::CCompletion(QObject *p)
: QCodeCompletionEngine(p),pPopup(0)
{
    addTrigger("(");

    QCodeCompletionEngine::triggerAction()->setShortcut(tr("Ctrl+Space"));
}

QString CCompletion::language() const
{
    return "C++";
}

QStringList CCompletion::extensions() const
{
    QStringList l;

    l	<< "c"
        << "h";

    return l;
}

QCodeCompletionEngine * CCompletion::clone()
{
    CCompletion *e = new CCompletion();

    foreach ( QString t, triggers() )
        e->addTrigger(t);

    emit cloned(e);

    return e;
}

QString CCompletion::getLastToken(const QDocumentCursor &c) {
    QString line = c.line().text();
    QString Token = "";
    if (line.size()>1) {
        int i = c.columnNumber()-1;
        while (QString("_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ").contains(line.at(i).toUpper())) {
            Token = line.at(i)+Token;
            i--;
            if (i<0) break;
        }
    }
    return Token;
}

void CCompletion::complete(const QDocumentCursor &c, const QString &trigger)
{
    if ( pPopup && pPopup->editor() != editor() )
    {
        delete pPopup;
        pPopup = 0;
    }

    if ( !pPopup )
    {
        pPopup = new QCodeCompletionWidget(editor());
    }

    pPopup->clear();
    pPopup->setCursor(editor()->cursor());

    QTime time;
    time.start();

    mainwindow->windowide->completionScan();




    //pPopup->setCompletions(nodes);

    pPopup->popup();

#if 1
    if ( trigger == "(" )
    {
        QStringList tips;

        //qDebug("fn %s", fn.constData());

        tips = mainwindow->windowide->getProc(getLastToken(c));

        //tips <<"deux";
        if ( tips.count() )
        {
            QRect r = editor()->cursorRect();
            QDocumentCursor cursor = editor()->cursor();
            QDocumentLine line = cursor.line();

            int hx = editor()->horizontalOffset(),
                cx = line.cursorToX(cursor.columnNumber());

            QCallTip *ct = new QCallTip(editor()->viewport());
            ct->move(cx - hx, r.y() + r.height());
            ct->setTips(tips);
            ct->show();
            ct->setFocus();

            #ifdef TRACE_COMPLETION
            qDebug("parsing + scoping + search + pre-display : elapsed %i ms", time.elapsed());
            #endif
        }
    }
#endif

}
