#ifndef WINDOWIDE_H
#define WINDOWIDE_H

#include <QMainWindow>


#include "lcc/lcpp.h"
#include "lcc/lcc.h"

#include "qcodeedit.h"
#include "ide/editorwidget.h"

class QFormatScheme;
class QLanguageFactory;
class CCompletion;
class QCodeNode;

namespace Ui {
    class WindowIDE;
}

class WindowIDE : public QMainWindow
{
    Q_OBJECT

public:
    explicit WindowIDE(QWidget *parent = 0);
    ~WindowIDE();


    void refreshFileList(void);

    CEditorWidget * createEditorTab(QString fname, QString text,bool = false);

    QMap<QString,QByteArray> mapSRC;
    QMap<QString,QByteArray> mapPP;
    QMap<QString,QByteArray> mapASM;
    QMap<QString,QByteArray> mapLM;

    QMap<QString,CEditorWidget *> editorMap;



    void addtargetCB(CPObject *pc);
    void removetargetCB(CPObject *pc);

    void createOutputTab(QString fname, QString text);



    QList<QCodeNode *> completionScan();
    QStringList getProc(QString s);
public slots:
    void saveAll(void);

private slots:
    void compile(void);
    void save(void);
    void load(QModelIndex index);
    void closeEditorTab(int);
    void newFile(void);
    void closeOutputTab(int index);
    void targetChange(QString);
    void installTo(CpcXXXX *pc, qint32 adr, QByteArray data);

signals:
    void newEmulatedPocket(CPObject *);
    void removeEmulatedPocket(CPObject *);

protected:

    void closeEvent(QCloseEvent *);

    QFormatScheme *m_formats;
    QLanguageFactory *m_languages;


private:
    void setupEditor(void);

    Ui::WindowIDE *ui;
    QFileSystemModel *model;
    CCompletion *e;

    QList<Cvar> varlist;
    QList<Cproc> proclist;
};

#endif // WINDOWIDE_H
