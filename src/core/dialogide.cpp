#include <QFile>

#include "dialogide.h"
#include "ide/highlighter.h"
#include "lcc/lcpp.h"
#include "lcc/lcc.h"
#include "lcc/pasm.h"



DialogIDE::DialogIDE( QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{



    setupUi(this);

    setupEditor();

    connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
    //textBrowser_2->setSource(QUrl("qrc:/POCKEMUL/pockemul/release_notes.html"));

    this->setWindowFlags(Qt::Window);
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
    QMap<QString,QByteArray> mapLM;
    mapSRC["test"] = src.toAscii();

    lcpp = new Clcpp(&mapSRC,&mapPP,this->modelCB->currentText());

    lcpp->run();

    outputpp->setPlainText(mapPP["test"]);


    lcc = new Clcc(&mapPP,&mapASM);
    connect(lcc,SIGNAL(outputSignal(QString,QString)),this,SLOT(output(QString,QString)));

    lcc->run();

    outputstd->setPlainText(mapASM["output"]);
    outputasm->setPlainText(mapASM["test.asm"]);

    Cpasm * pasm = new Cpasm(&mapASM,&mapLM);

    pasm->parsefile("BAS",mapASM["test.asm"]);
    pasm->savefile("BAS");
    outputlm->setPlainText(mapLM["BAS"]);
}

void DialogIDE::output(QString f,QString s) {
    outputstd->insertPlainText(s);
    //outputstd->append(s);
    outputstd->update();
}
