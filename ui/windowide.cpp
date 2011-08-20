
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
#include "qlanguagedefinition.h"
#include "qhexpanel.h"
#include "qoutpanel.h"

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
//    connect(ui->installPB,SIGNAL(clicked()),this,SLOT(inject()));
//    connect(ui->savePB,SIGNAL(clicked()),this,SLOT(save()));
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

    m_formats = new QFormatScheme(":/QXS/formats.qxf", this);
    QDocument::setDefaultFormatScheme(m_formats);

    QLineMarksInfoCenter::instance()->loadMarkTypes(":QXS/marks.qxm");

    m_languages = new QLanguageFactory(m_formats, this);
    m_languages->addDefinitionPath(":QXS");



    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);


    refreshFileList();
}


void WindowIDE::compile(void) {
#if 1

    mapSRC.clear();
    mapPP.clear();
    mapASM.clear();
    mapLM.clear();

    CEditorWidget *locEditorWidget = ((CEditorWidget*)ui->tabWidget->currentWidget());

    QString source = locEditorWidget->m_editControl->editor()->text();
    QString sourcefname=locEditorWidget->m_editControl->editor()->fileName();
    QFileInfo fInfo(sourcefname);
    if (locEditorWidget->m_editControl->editor()->languageDefinition()->language()=="C++") {
        mapSRC[sourcefname] = source.toAscii();
        Clcpp *lcpp = new Clcpp(&mapSRC,&mapPP,"PC-1350");
        lcpp->run();
        createTab(fInfo.baseName()+".pp",mapPP[sourcefname]);
        Clcc *lcc = new Clcc(&mapPP,&mapASM);
        lcc->run();
        //ui->outputstd->setPlainText(mapASM["output"]);

        //ui->outputasm->setPlainText(mapASM["test.asm"]);


        createTab(fInfo.baseName()+".asm",mapASM[fInfo.baseName()+".asm"]);

        CEditorWidget *currentWidget = ((CEditorWidget*)ui->tabWidget->currentWidget());
        QOutPanel *outpanel = new QOutPanel();
        currentWidget->m_editControl
                ->addPanel(outpanel, QCodeEdit::South, true);
        outpanel->out->setText(mapASM["output"]);
    }

    if (locEditorWidget->m_editControl->editor()->languageDefinition()->language()=="ASM") {
        CEditorWidget *currentWidget = locEditorWidget;

        mapSRC[sourcefname] = source.toAscii();
        Cpasm * pasm = new Cpasm(&mapSRC,&mapLM);
        pasm->parsefile("BAS",mapSRC[sourcefname]);
        pasm->savefile("BAS");
        pasm->savefile("BIN");
        pasm->savefile("HEX");

        createTab(fInfo.baseName()+".bas",mapLM["BAS"]);

        createTab(fInfo.baseName()+".output",mapLM["output"]);

        //createTab(fInfo.baseName()+".bin",mapLM["BIN"]);
        //ui->tabWidget->setCurrentWidget(currentWidget);
        currentWidget = ((CEditorWidget*)ui->tabWidget->currentWidget());
        QHexPanel *hexpanel = new QHexPanel();


        currentWidget->m_editControl
                ->addPanel(hexpanel, QCodeEdit::South, true);
        hexpanel->hexeditor->setData(mapLM["BIN"]);
        hexpanel->hexeditor->setCursorPosition(0,BINEditor::BinEditor::MoveAnchor);
        connect(this,SIGNAL(newEmulatedPocket(CPObject*)),hexpanel,SLOT(newPocket(CPObject*)));

        hexpanel->startadr = mapLM["_ORG"].trimmed().toLong();

//        MSG_ERROR("*"+mapLM["_ORG"]+"*");
//        MSG_ERROR(QString("%1").arg(hexpanel->startadr));
    }
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


void WindowIDE::createTab(QString fname, QString text) {
    CEditorWidget *locEditorWidget = new CEditorWidget();
    ui->tabWidget->insertTab(0,locEditorWidget,fname);
    m_languages->setLanguage(locEditorWidget->m_editControl->editor(), fname);
    locEditorWidget->m_editControl->editor()->setText(text);
    locEditorWidget->m_editControl->editor()->setFileName(fname);
    editorMap.insert(fname,locEditorWidget);
    ui->tabWidget->setCurrentIndex(0);
}

void WindowIDE::output(QString f,QString s) {

}



void WindowIDE::addtargetCB(CPObject *pc) {
    emit newEmulatedPocket(pc);
}


void WindowIDE::installTo(CpcXXXX * pc,qint32 adr, QByteArray data ) {

    QDataStream in(data);
    in.readRawData ((char *) &pc->mem[adr],
                    data.size() );
    QMessageBox::about(mainwindow,"Transfert",tr("LM stored at %1").arg(adr));
}

void WindowIDE::save(void) {

}

void WindowIDE::refreshFileList(void) {
    QDir dir;
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList(QStringList() << "*.c" << "*.asm");
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
