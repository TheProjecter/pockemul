/*!
    \file windowide.cpp
    \brief Implementation of the WindowIDE class
*/

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
#include "ccompletion.h"

#include "qcodemodel2/qcodenode.h"

extern QList<CPObject *> listpPObject; /*!< TODO */

/*!
    \class WindowIDE
    \brief IDE main class

    The WindowIDE class ...
*/

/*!
 \brief IDE main class constructor

 \param parent
*/
WindowIDE::WindowIDE(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowIDE)
{
    ui->setupUi(this);

    tabifyDockWidget(ui->projectDockWidget,ui->filesDockWidget);
    this->setWindowTitle(tr("PockEmul Integrated Development Environment"));
    this->setAttribute(Qt::WA_DeleteOnClose,true);

    setupEditor();

    connect(ui->actionCompile, SIGNAL(triggered()), this, SLOT(compile()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(save()));
    connect(ui->actionSave_All,SIGNAL(triggered()),this,SLOT(saveAll()));
    //connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(load(QListWidgetItem*)));
    connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(load(QModelIndex)));
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeEditorTab(int)));
    connect(ui->outputtabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeOutputTab(int)));
    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(newFile()));

    connect(mainwindow,SIGNAL(DestroySignal(CPObject*)),this,SLOT(DestroySlot(CPObject*)));
}

/*!
 \brief desctructeur

*/
WindowIDE::~WindowIDE()
{
    delete m_formats;
    delete m_languages;
    foreach (CEditorWidget *value, editorMap) {
         delete value;
    }
    delete ui;

}




/*!
 \brief

*/
void WindowIDE::setupEditor()
{

    m_formats = new QFormatScheme(":/QXS/formats.qxf", this);
    QDocument::setDefaultFormatScheme(m_formats);

    QLineMarksInfoCenter::instance()->loadMarkTypes(":QXS/marks.qxm");

    m_languages = new QLanguageFactory(m_formats, this);
    m_languages->addDefinitionPath(":QXS");



    refreshFileList();

    ui->vardockWidget->hide();
    ui->procdockWidget->hide();

}

QList<QCodeNode *> WindowIDE::completionScan(QEditor *e) {

#ifndef Q_OS_ANDROID

    QMap<QString,QByteArray> mapSRC;
    QMap<QString,QByteArray> mapPP;
    QMap<QString,QByteArray> mapASM;
    //CEditorWidget *locEditorWidget = ((CEditorWidget*)ui->tabWidget->currentWidget());

    QString source = "#include <internal.h>\r\n"+e->text();
    QString sourcefname=e->fileName();
    QFileInfo fInfo(sourcefname);

    if (e->languageDefinition()->language()=="C++") {
        mapSRC[sourcefname] = source.toAscii();
        Clcpp *lcpp = new Clcpp(&mapSRC,&mapPP,ui->targetComboBox->currentText(),false);
        lcpp->pStdLibs->LoadLibs();
        lcpp->run();
        this->doxygenlist = lcpp->getDoxygenList();
        //createEditorTab(fInfo.baseName()+".xml",mapPP["DOxygen"]);
        //createOutputTab("PP Compiler :"+fInfo.fileName(),mapPP["output"]);
        Clcc *lcc = new Clcc(&mapPP,&mapASM,false);
        lcc->FirstScan(mapPP[sourcefname]);
        //lcc->run();

        // recuperer var et proc
        //lcc->varlist

        //lcc->proclist
        lcc->printvarlist("variables");
        lcc->printproclist("procedures");

        this->varlist.clear();
        this->proclist.clear();

        this->varlist = lcc->varlist;
        this->proclist = lcc->proclist;


        QList<QCodeNode *> nodes;

        QCodeNode *n = new QCodeNode();

        for (int i=0;i<varlist.size();i++) {
            Cvar v = varlist.at(i);
            QCodeNode *vn = new QCodeNode();
            vn->roles = "v@"+v.varname+"@"+v.typ;
            vn->attach(n);
        }

        //    fct->roles = QByteArray("f@")
        //                + name
        //                + "@"
        //                + retval
        //                + "@"
        //                + QByteArray::number(visibility)
        //                + "@"
        //                ;

        for (int i=0;i<proclist.size();i++) {
            Cproc p = proclist.at(i);
            QCodeNode *pn = new QCodeNode();
            //name
            pn->roles = "f@"+p.ProcName+"@";
            //return
            if (p.hasreturn) {
                if (p.ReturnIsWord) pn->roles+="word";
                else pn->roles+="byte";
            }
            //visibility + template
            pn->roles+="@@@";
            //arguments
            if (p.ParCnt > 0) pn->roles+=p.Params+"@";

            pn->attach(n);
        }
        nodes.append(n);

        ui->vartextEdit->setText(mapASM["variables"]);
        ui->proctextEdit->setText(mapASM["procedures"]);

        return nodes;
    }
#endif
}

QStringList WindowIDE::getProc(QString s) {
    QString sr = "";
    for (int i= 0; i< proclist.size();i++) {
        if (proclist.at(i).ProcName == s) {
            if (proclist.at(i).hasreturn) {
                if (proclist.at(i).ReturnIsWord) sr+="word ";
                else sr+="byte ";
            }
            sr += proclist.at(i).ProcName+"( ";

            if (proclist.at(i).ParCnt > 0) sr+=proclist.at(i).Params;
            sr+=")";
            return QStringList() << sr;
        }
    }
    return QStringList();
}

/*!
 \brief lance le processus de compilation adéquate en fonction de l'extension du fichier

*/
void WindowIDE::compile(void) {
#ifndef Q_OS_ANDROID
    mapSRC.clear();
    mapPP.clear();
    mapASM.clear();
    mapLM.clear();

    save();

    CEditorWidget *locEditorWidget = ((CEditorWidget*)ui->tabWidget->currentWidget());

    QString source = "#include <internal.h>\r\n"+locEditorWidget->m_editControl->editor()->text();
    QString sourcefname=locEditorWidget->m_editControl->editor()->fileName();
    QFileInfo fInfo(sourcefname);

    if (locEditorWidget->m_editControl->editor()->languageDefinition()->language()=="C++") {
        mapSRC[sourcefname] = source.toAscii();
        Clcpp *lcpp = new Clcpp(&mapSRC,&mapPP,ui->targetComboBox->currentText());
        lcpp->pStdLibs->LoadLibs();
        lcpp->run();
        //createEditorTab(fInfo.baseName()+".pp",mapPP[sourcefname]);
        //createOutputTab("PP Compiler :"+fInfo.fileName(),mapPP["output"]);
#if 1
        Clcc *lcc = new Clcc(&mapPP,&mapASM);
        lcc->run();
        createEditorTab(fInfo.baseName()+".log",mapASM["LOG"]);

        createEditorTab(fInfo.baseName()+".asm",mapASM[fInfo.baseName()+".asm"]);

        createOutputTab("C Compiler :"+fInfo.fileName(),mapASM["output"]);
#endif
    }

    if (locEditorWidget->m_editControl->editor()->languageDefinition()->language()=="ASM") {
        CEditorWidget *currentWidget = locEditorWidget;

        mapSRC[sourcefname] = source.toAscii();
        Cpasm * pasm = new Cpasm(&mapSRC,&mapLM);
        pasm->run("BAS",mapSRC[sourcefname]);
        pasm->savefile("BAS");
        pasm->savefile("BIN");
        pasm->savefile("HEX");

        createEditorTab(fInfo.baseName()+".bas",mapLM["BAS"]);

        createOutputTab("ASM Compiler :"+fInfo.fileName(),mapLM["output"]);

        currentWidget = ((CEditorWidget*)ui->tabWidget->currentWidget());
        QHexPanel *hexpanel = new QHexPanel();


        currentWidget->m_editControl
                ->addPanel(hexpanel, QCodeEdit::South, true);
        hexpanel->startadr = mapLM["_ORG"].trimmed().toLong();
        hexpanel->hexeditor->setData(mapLM["BIN"],hexpanel->startadr);
        hexpanel->hexeditor->setCursorPosition(0,BINEditor::BinEditor::MoveAnchor);
        connect(this,SIGNAL(newEmulatedPocket(CPObject*)),hexpanel,SLOT(newPocket(CPObject*)));
        connect(this,SIGNAL(removeEmulatedPocket(CPObject*)),hexpanel,SLOT(removePocket(CPObject*)));
        connect(hexpanel,SIGNAL(installTo(CpcXXXX*,qint32,QByteArray)),this,SLOT(installTo(CpcXXXX*,qint32,QByteArray)));




//        MSG_ERROR("*"+mapLM["_ORG"]+"*");
//        MSG_ERROR(QString("%1").arg(hexpanel->startadr));
    }
#endif
}


/*!
 \brief Add a new tab to the Editor panel

 \param fname   Filename of the new document
 \param text    document content
 \param load    if true then load the text from disk
*/
CEditorWidget * WindowIDE::createEditorTab(QString fname, QString text,bool load) {

    CEditorWidget *locEditorWidget = new CEditorWidget(this);
    ui->tabWidget->insertTab(0,locEditorWidget,QFileInfo(fname).fileName());
    ui->tabWidget->setTabToolTip(0,QFileInfo(fname).absoluteFilePath());
    m_languages->setLanguage(locEditorWidget->m_editControl->editor(), fname);

    e = new CCompletion();
    e->triggerAction()->setShortcut(tr("Ctrl+Space"));

    //connect(locEditorWidget->m_editControl->editor(),SIGNAL(cursorPositionChanged()),e,SLOT(complete()));

    //e->setEditor(locEditorWidget->m_editControl->editor());
    m_languages->addCompletionEngine(e);
    locEditorWidget->m_editControl->editor()->setCompletionEngine(e->clone());

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(14);
    locEditorWidget->m_editControl->editor()->setFont(font);

    locEditorWidget->m_editControl->editor()->setText(text);
    locEditorWidget->m_editControl->editor()->setFileName(fname);
    if (load) locEditorWidget->m_editControl->editor()->load(fname);
    editorMap.insert(fname,locEditorWidget);
    ui->tabWidget->setCurrentIndex(0);

    return locEditorWidget;
}

/*!
 \brief Add a new tab to the Output Panel

 \param fname   The tab name
 \param text    Text displayed into the new tab
*/
void WindowIDE::createOutputTab(QString fname, QString text) {
    QTextEdit *locTextEdit = new QTextEdit();
    ui->outputtabWidget->insertTab(0,locTextEdit,fname);
    locTextEdit->setText(text);
    ui->outputtabWidget->setCurrentIndex(0);
}


/*!
 \brief

 \param pc
*/
void WindowIDE::addtargetCB(CPObject *pc) {
    emit newEmulatedPocket(pc);
}

void WindowIDE::removetargetCB(CPObject *pc) {
    emit removeEmulatedPocket(pc);
}

/*!
 \brief Charge le binaire data dans la mémoire du Pocket émulé.

 \param pc      Le pocket cible.
 \param adr     Adresse de chargement des données.
 \param data    Données binaires à charger.
*/
void WindowIDE::installTo(CpcXXXX * pc,qint32 adr, QByteArray data ) {
qint32 targetAdr = adr;
    if (!(ui->adrLineEdit->text().isEmpty())) {
        targetAdr = ui->adrLineEdit->text().toLong(0,16);
    }
    if (pc->Mem_Load(targetAdr,data)) {
        QMessageBox::about(mainwindow,"Transfert",tr("LM stored at %1").arg(targetAdr));
    }
}

void WindowIDE::DestroySlot(CPObject *pObject)
{
    removetargetCB(pObject);
}

/*!
 \brief Sauvegarde le fichier sur disque.

*/
void WindowIDE::save(void) {
    CEditorWidget *locEditorWidget = (CEditorWidget*)ui->tabWidget->currentWidget();
    if (locEditorWidget->m_editControl->editor()->isContentModified()) {
        // save it
        locEditorWidget->m_editControl->editor()->save();
    }
}

/*!
 \brief Sauvegarde tous les fichiers ouverts sur disque.

 \fn WindowIDE::saveAll
*/
void WindowIDE::saveAll(void) {
    for (int i=0; i < ui->tabWidget->count();i++) {
        CEditorWidget *locEditorWidget = (CEditorWidget*)ui->tabWidget->widget(i);
        if (locEditorWidget->m_editControl->editor()->isContentModified()) {
            // save it
            locEditorWidget->m_editControl->editor()->save();
        }
    }
}
/*!
 \brief

*/
void WindowIDE::refreshFileList(void) {
//    QDir dir;
//    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
//    dir.setSorting(QDir::Size | QDir::Reversed);

//    QFileInfoList list = dir.entryInfoList(QStringList() << "*.c" << "*.asm");
//    //QFileInfoList list = dir.entryInfoList();
//    for (int i = 0; i < list.size(); ++i) {
//        QFileInfo fileInfo = list.at(i);
//        ui->listWidget->addItem(fileInfo.fileName());
//    }
    model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    model->setNameFilters(QStringList() << "*.c" << "*.asm" << "*.h");
    model->setNameFilterDisables(false);
    ui->treeView->setModel(model);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->treeView->header()->hide();
    ui->treeView->setRootIndex(model->index(QDir::currentPath()));
}

/*!
 \brief

 \param id
*/
void WindowIDE::load(QModelIndex index) {
    //QMessageBox::about(this,"test",model->fileName(index)+" - "+model->filePath(index));
    // check if file still open
    if (editorMap.contains(model->filePath(index))) {
            ui->tabWidget->setCurrentWidget(editorMap.value(model->filePath(index)));
    }
    else {
    // If not create a new tab
        createEditorTab(model->filePath(index),"",true);

    }

}

/*!
 \brief

 \param index
*/
void WindowIDE::closeEditorTab(int index) {
    CEditorWidget *locEditorWidget = (CEditorWidget*)ui->tabWidget->widget(index);
    if (locEditorWidget->m_editControl->editor()->isContentModified()) {
        // save it
        locEditorWidget->m_editControl->editor()->save();
    }
    ui->tabWidget->removeTab(index);
    editorMap.remove(locEditorWidget->m_editControl->editor()->fileName());
}

/*!
 \brief

 \fn WindowIDE::closeOutputTab
 \param index
*/
void WindowIDE::closeOutputTab(int index) {
    ui->outputtabWidget->removeTab(index);
}
/*!
 \brief

 \param event
*/
void WindowIDE::closeEvent(QCloseEvent *event)
{
    //Check unsaved data
    mainwindow->windowide = 0;
    event->accept();

}

/*!
 \brief

*/
void WindowIDE::newFile()
{
    QString fileName = QFileDialog::getSaveFileName(
                        this,
                        tr("Choose a file"),
                        ".",
                        tr("C source (*.c *.h);;ASM source (*.asm)"),
                        new QString("(*.c)"));

    createEditorTab(fileName,"");

}

void WindowIDE::targetChange(QString m)
{

}

Cproc WindowIDE::getProcObj(QString s)
{
    for (int i= 0; i< proclist.size();i++) {
        if (proclist.at(i).ProcName == s) {
            return proclist.at(i);
        }
    }
    return Cproc();
}

CDOxyItem * WindowIDE::getDOxygenInfo(QString s)
{
    for (int i=0;i< doxygenlist.size();i++) {
        if (doxygenlist.at(i)->fn.trimmed()==s) return doxygenlist.at(i);
    }
    return 0;
}
