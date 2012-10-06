#ifndef DIALOGVKEYBOARD_H
#define DIALOGVKEYBOARD_H

#include <QDialog>
#include <QHash>

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
private slots:
    void InsertKeySlot(QListWidgetItem*);
    void senData();

private:
    Ui::DialogVKeyboard *ui;
    CPObject *pPC;
    QHash<QString, int> dict;
};

#endif // DIALOGVKEYBOARD_H
