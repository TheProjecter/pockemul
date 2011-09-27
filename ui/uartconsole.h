#ifndef UARTCONSOLE_H
#define UARTCONSOLE_H

#include <QWidget>

namespace Ui {
    class CUartConsole;
}

class CUartConsole : public QWidget
{
    Q_OBJECT

public:
    explicit CUartConsole(QWidget *parent = 0);
    ~CUartConsole();

private:
    Ui::CUartConsole *ui;
};

#endif // UARTCONSOLE_H
