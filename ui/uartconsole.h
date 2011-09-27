#ifndef UARTCONSOLE_H
#define UARTCONSOLE_H

#include <QDialog>

namespace Ui {
    class CUartConsole;
}

class CUartConsole : public QDialog
{
    Q_OBJECT

public:
    explicit CUartConsole(QWidget *parent = 0);
    ~CUartConsole();
public slots:
    void newOutputByte(qint8 data);
private:
    Ui::CUartConsole *ui;
};

#endif // UARTCONSOLE_H
