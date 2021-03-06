#ifndef DIALOGDASM_H
#define DIALOGDASM_H

#include <QDialog>
class QListWidgetItem;
class CpcXXXX;
class CregCPU;

namespace BINEditor {
    class BinEditor;
}

namespace Ui {
    class DialogDasm;
}

class DialogDasm : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDasm(QWidget *parent = 0);
    ~DialogDasm();


    bool IsAdrInList(qint32 adr);
    CpcXXXX * pPC;
    int		Index;
    quint32	MaxAdr;
    quint32	NextMaxAdr;
    bool imem;

    void resizeEvent(QResizeEvent *event);
    void selectRow(int index);
    quint16 getValue();
private:
    Ui::DialogDasm *ui;
    BINEditor::BinEditor *imemHexEditor;
    BINEditor::BinEditor *memHexEditor;
    CregCPU *regwidget;

public slots:
    void RefreshDasm();
    void loadImem();
    void loadMem();
    void start();
    void stop();
    void step();
    void stepOver();
    void addBreakPoint();
    void breakPointChanged(QListWidgetItem* item);
};

#endif // DIALOGDASM_H
