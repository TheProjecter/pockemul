#ifndef DIALOGDASM_H
#define DIALOGDASM_H

#include <QDialog>
class CpcXXXX;

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
    qint32	MaxAdr;
    qint32	NextMaxAdr;

    void resizeEvent(QResizeEvent *event);
private:
    Ui::DialogDasm *ui;
    BINEditor::BinEditor *imemHexEditor;
    BINEditor::BinEditor *memHexEditor;

public slots:
    void RefreshDasm();
    void loadImem();
    void loadMem();
    void start();
    void stop();
    void step();
};

#endif // DIALOGDASM_H
