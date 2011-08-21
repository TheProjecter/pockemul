#ifndef WINDOWIDE_H
#define WINDOWIDE_H

#include <QMainWindow>


#include "lcc/lcpp.h"
#include "lcc/lcc.h"

#include "qcodeedit.h"
#include "ide/editorwidget.h"

class QFormatScheme;
class QLanguageFactory;

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

    void createEditorTab(QString fname, QString text,bool = false);

    QMap<QString,QByteArray> mapSRC;
    QMap<QString,QByteArray> mapPP;
    QMap<QString,QByteArray> mapASM;
    QMap<QString,QByteArray> mapLM;

    QMap<QString,CEditorWidget *> editorMap;

    void installTo(CpcXXXX *pc, qint32 adr, QByteArray data);

    void addtargetCB(CPObject *pc);

    void createOutputTab(QString fname, QString text);

public slots:
    void saveAll(void);

private slots:
    void compile(void);
    void save(void);
    void load(QListWidgetItem* id);
    void closeEditorTab(int);
    void newFile(void);
    void closeOutputTab(int index);

signals:
    void newEmulatedPocket(CPObject *);

protected:

    void closeEvent(QCloseEvent *);

    QFormatScheme *m_formats;
    QLanguageFactory *m_languages;


private:
    void setupEditor(void);

    Ui::WindowIDE *ui;
};

#endif // WINDOWIDE_H
