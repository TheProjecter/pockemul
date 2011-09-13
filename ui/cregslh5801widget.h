#ifndef CREGSLH5801WIDGET_H
#define CREGSLH5801WIDGET_H

#include <QWidget>

#include "ui/cregcpu.h"
class CCPU;

namespace Ui {
    class Cregslh5801Widget;
}

class Cregslh5801Widget : public CregCPU
{
    Q_OBJECT

public:
    explicit Cregslh5801Widget(QWidget *parent = 0,CCPU *pCPU = 0);
    ~Cregslh5801Widget();

    void refresh();

private:
    Ui::Cregslh5801Widget *ui;
};

#endif // CREGSLH5801WIDGET_H
