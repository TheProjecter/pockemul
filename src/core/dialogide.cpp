#include <QFile>

#include "dialogide.h"
#include "ide/highlighter.h"
#include "lcc/lcpp.h"
#include "lcc/lcc.h"

DialogIDE::DialogIDE( QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{



    setupUi(this);

    setupEditor();

    connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
    //textBrowser_2->setSource(QUrl("qrc:/POCKEMUL/pockemul/release_notes.html"));
}

void DialogIDE::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);


    editor->setFont(font);

    highlighter = new Highlighter(editor->document());

    QFile file("mainwindow.h");
    if (file.open(QFile::ReadOnly | QFile::Text))
        editor->setPlainText(file.readAll());
}

void DialogIDE::start(void) {
    QString src = editor->toPlainText();

    QMap<QString,QByteArray> mapSRC;
    QMap<QString,QByteArray> mapPP;
    QMap<QString,QByteArray> mapASM;
    mapSRC["test"] = src.toAscii();

    lcpp = new Clcpp(&mapSRC,&mapPP);

    lcpp->run();

    outputpp->setText(mapPP["test"]);


    lcc = new Clcc(&mapPP,&mapASM);
    connect(lcc,SIGNAL(outputSignal(QString,QString)),this,SLOT(output(QString,QString)));

    lcc->FirstScan(mapPP["test"]);
    lcc->SecondScan("bof");
    outputasm->setText(mapASM["asm"]);
}

void DialogIDE::output(QString f,QString s) {
    outputstd->insertPlainText(s);
    //outputstd->append(s);
    outputstd->update();
}
