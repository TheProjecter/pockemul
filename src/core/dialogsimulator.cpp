#include <QTextCursor>
#include <QCloseEvent>
#include <QAbstractButton>
#include <QFileDialog>
#include <QTextStream>

#include "dialogsimulator.h"
#include "common.h"
#include "pcxxxx.h"
#include "mainwindowpockemul.h"

extern	MainWindowPockemul* mainwindow;

//
DialogSimulator::DialogSimulator( QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
    setupUi(this);
    setFocusPolicy(Qt::StrongFocus);

    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(clicked(QAbstractButton*)));

    currentIndex=0;

    pCeSimu = (Ccesimu *) parent;

}
//
void DialogSimulator::refresh( void)
{



}



void DialogSimulator::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void DialogSimulator::clicked( QAbstractButton * button) {
    if ( buttonBox->standardButton(button) == QDialogButtonBox::Apply ) {
            // code pour Apply

            QString header = "";//function main() {";
            QString footer = "";// return 'OK';}";
            pCeSimu->script = new QScriptValue(pCeSimu->engine->evaluate(header + textEdit->toPlainText() + footer));
            pCeSimu->mainfunction = new QScriptValue(pCeSimu->engine->globalObject().property("run"));
            //QString s = pCeSimu->mainfunction->call(QScriptValue()).toString();

            //MSG_ERROR(s);

        }
    else
    if ( buttonBox->standardButton(button) == QDialogButtonBox::Open ) {
        // code pour Apply

        QString fileName = QFileDialog::getOpenFileName(
                mainwindow,
                tr("Choose a file"),
                ".",
                tr("PockEmul sessions (*.qs)"));

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(mainwindow,tr("PockEmul"),
                                 tr("Cannot read file %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
            return ;
        }
        textEdit->setPlainText(QString(file.readAll()));
    }
    else
    if ( buttonBox->standardButton(button) == QDialogButtonBox::Save ) {
        // code pour Apply
        QString fn = QFileDialog::getSaveFileName(
                mainwindow,
                tr("Choose a filename to save the script"),
                ".",
                tr("Session File (*.qs)"));

        QFile f(fn);
        if (f.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream out(&f);
            out << textEdit->toPlainText();
        }else {
            MSG_ERROR("ERROR saving script")
        }

    }
    else {
        // code pour Reset
    }
}
