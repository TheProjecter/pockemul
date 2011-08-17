
#include <QFile>

#include "windowide.h"
#include "ui_windowide.h"

#include "ide/highlighter.h"
#include "lcc/lcpp.h"
#include "lcc/lcc.h"
#include "lcc/pasm.h"
#include "pobject.h"
#include "pcxxxx.h"


#include "qformatscheme.h"
#include "qdocument.h"
#include "qlinemarksinfocenter.h"
#include "qeditor.h"
#include "qcodeedit.h"
#include "qlanguagefactory.h"

extern QList<CPObject *> listpPObject;

WindowIDE::WindowIDE(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowIDE)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("Integrated Development Environment"));
    this->setAttribute(Qt::WA_DeleteOnClose,true);

    setupEditor();

    connect(ui->actionCompile, SIGNAL(triggered()), this, SLOT(compile()));
    connect(ui->installPB,SIGNAL(clicked()),this,SLOT(inject()));
    connect(ui->savePB,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(load(QListWidgetItem*)));
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));

}

WindowIDE::~WindowIDE()
{
    delete m_formats;
    delete m_languages;
    foreach (CEditorWidget *value, editorMap) {
         delete value;
    }
    delete ui;

}




void WindowIDE::setupEditor()
{

    m_formats = new QFormatScheme("qxs/formats.qxf", this);
    QDocument::setDefaultFormatScheme(m_formats);

    QLineMarksInfoCenter::instance()->loadMarkTypes("qxs/marks.qxm");

    m_languages = new QLanguageFactory(m_formats, this);
    m_languages->addDefinitionPath("qxs");


    fill_inject();

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);


    ui->editor->setFont(font);
    ui->editor->setTabStopWidth(40);

    highlighter = new Highlighter(ui->editor->document());

    refreshFileList();
}


void WindowIDE::compile(void) {
#if 1

    mapSRC.clear();
    mapPP.clear();
    mapASM.clear();

    QString source = ((CEditorWidget*)ui->tabWidget->currentWidget())->m_editControl->editor()->text();
    QString sourcefname=((CEditorWidget*)ui->tabWidget->currentWidget())->m_editControl->editor()->fileName();

    mapSRC[sourcefname] = source.toAscii();
    Clcpp *lcpp = new Clcpp(&mapSRC,&mapPP,this->ui->modelCB->currentText());
    lcpp->run();
    Clcc *lcc = new Clcc(&mapPP,&mapASM);
    lcc->run();
    //ui->outputstd->setPlainText(mapASM["output"]);
    //ui->outputasm->setPlainText(mapASM["test.asm"]);
    CEditorWidget *locEditorWidget = new CEditorWidget();
    QFileInfo fInfo(sourcefname);
    ui->tabWidget->insertTab(0,locEditorWidget,fInfo.baseName()+".asm");

    m_languages->setLanguage(locEditorWidget->m_editControl->editor(), fInfo.baseName()+".asm");
    locEditorWidget->m_editControl->editor()->setText(mapASM[fInfo.baseName()+".asm"]);
    editorMap.insert(fInfo.baseName()+".asm",locEditorWidget);
    ui->tabWidget->setCurrentIndex(0);

#else
    QString src = ui->editor->toPlainText();

    mapSRC.clear();
    mapPP.clear();
    mapASM.clear();
    mapLM.clear();

    mapSRC["test"] = src.toAscii();

    Clcpp *lcpp = new Clcpp(&mapSRC,&mapPP,this->ui->modelCB->currentText());
    lcpp->run();
    ui->outputpp->setPlainText(mapPP["test"]);

    Clcc *lcc = new Clcc(&mapPP,&mapASM);
 //   connect(lcc,SIGNAL(outputSignal(QString,QString)),this,SLOT(output(QString,QString)));
    lcc->run();

    ui->outputstd->setPlainText(mapASM["output"]);
    ui->outputasm->setPlainText(mapASM["test.asm"]);

    Cpasm * pasm = new Cpasm(&mapASM,&mapLM);
    pasm->parsefile("BAS",mapASM["test.asm"]);
    pasm->savefile("BAS");
    pasm->savefile("BIN");

    ui->outputlm->setPlainText(mapLM["BAS"]);
    ui->outputstd->appendPlainText("\r\r\r"+mapLM["output"]);
#endif
}

void WindowIDE::output(QString f,QString s) {
    ui->outputstd->insertPlainText(s);
    //outputstd->append(s);
    ui->outputstd->update();
}

void WindowIDE::fill_inject(void) {
    // update the injectCB ComboBox
    ui->injectCB->clear();
    for (int i = 0; i < listpPObject.size();i++) {
        CPObject *p = listpPObject.at(i);
        p->getName();
        ui->injectCB->addItem(p->getName(),tr("%1").arg((long)p));
    }
}

void WindowIDE::inject(void) {

    int index = ui->injectCB->currentIndex();
    CpcXXXX *pc = (CpcXXXX *) ui->injectCB->itemData(index).toString().toULongLong();
    bool ok;
    int orig = ui->origEdit->text().toInt(&ok,16);

    QDataStream in(mapLM["BIN"]);
    in.readRawData ((char *) &pc->mem[orig],
                    mapLM["BIN"].size() );
    QMessageBox::about(mainwindow,"Transfert",tr("LM stored at %1").arg(orig));

}

void WindowIDE::save(void) {

}

void WindowIDE::refreshFileList(void) {
    QDir dir;
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList(QStringList("*.c;*.asm"));
    //QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        ui->listWidget->addItem(fileInfo.fileName());
    }
}

void WindowIDE::load(QListWidgetItem* id) {

    // check if file still open
    if (editorMap.contains(id->text())) {
            ui->tabWidget->setCurrentWidget(editorMap.value(id->text()));
    }
    else {
    // If not create a new tab
        CEditorWidget *locEditorWidget = new CEditorWidget();
        ui->tabWidget->insertTab(0,locEditorWidget,id->text());

        m_languages->setLanguage(locEditorWidget->m_editControl->editor(), id->text());
        locEditorWidget->m_editControl->editor()->load(id->text());
        editorMap.insert(id->text(),locEditorWidget);
        ui->tabWidget->setCurrentIndex(0);
    }
}

void WindowIDE::closeTab(int index) {
    CEditorWidget *locEditorWidget = (CEditorWidget*)ui->tabWidget->widget(index);
        if (locEditorWidget->m_editControl->editor()->isContentModified()) {
            // save it
            locEditorWidget->m_editControl->editor()->save();
        }
    ui->tabWidget->removeTab(index);
    editorMap.remove(locEditorWidget->m_editControl->editor()->fileName());
}

void WindowIDE::closeEvent(QCloseEvent *event)
{
    //Check unsaved data
    mainwindow->windowide = 0;
    event->accept();

}
