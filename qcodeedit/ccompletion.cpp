#include "ccompletion.h"
#include "qcalltip.h"
#include "qeditor.h"
#include "qdocumentline.h"
#include "qcodecompletionwidget.h"
#include <QTime>
#include <QAction>

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

    pPopup->popup();

#if 1
    QRect r = editor()->cursorRect();
    QDocumentCursor cursor = editor()->cursor();
    QDocumentLine line = cursor.line();

    int hx = editor()->horizontalOffset(),
        cx = line.cursorToX(cursor.columnNumber());

    QCallTip *ct = new QCallTip(editor()->viewport());
    ct->move(cx - hx, r.y() + r.height());
    ct->setTips(QStringList() << "un"<<"deux"<<"trois");
    ct->show();
    ct->setFocus();
#endif
}
