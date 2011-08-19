#ifndef WINDOWIDE_H
#define WINDOWIDE_H

#include <QMainWindow>


#include "ide/highlighter.h"
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

    void setupEditor(void);
    void refreshFileList(void);

    void createTab(QString fname, QString text);

    Highlighter *highlighter;

    QMap<QString,QByteArray> mapSRC;
    QMap<QString,QByteArray> mapPP;
    QMap<QString,QByteArray> mapASM;
    QMap<QString,QByteArray> mapLM;

    QMap<QString,CEditorWidget *> editorMap;

    void installTo(CpcXXXX *pc, qint32 adr, QByteArray data);

    void addtargetCB(CPObject *pc);

private slots:
    void compile(void);
    void output(QString,QString);
    void save(void);
    void load(QListWidgetItem* id);
    void closeTab(int);

signals:
    void newEmulatedPocket(CPObject *);

protected:

    void closeEvent(QCloseEvent *);

    QFormatScheme *m_formats;
    QLanguageFactory *m_languages;


private:
    Ui::WindowIDE *ui;
};

#endif // WINDOWIDE_H
