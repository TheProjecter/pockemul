#ifndef DIALOGVKEYBOARD_H
#define DIALOGVKEYBOARD_H

#include <QDialog>
#include <QHash>

#include "Keyb.h"

class CPObject;
class QListWidgetItem;

namespace Ui {
class DialogVKeyboard;
}

class DialogVKeyboard : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogVKeyboard(QWidget *parent = 0);
    ~DialogVKeyboard();
    
    void PopulateKeyList();
    int convertKeyCode(int c);
    void processEscKey(QString word);
private slots:
    void InsertKeySlot(QListWidgetItem*);
    void senData();

private:
    Ui::DialogVKeyboard *ui;
    CPObject *pPC;
    QHash<QString, CKey *> dict;
    QList<int> final;
};

#endif // DIALOGVKEYBOARD_H
