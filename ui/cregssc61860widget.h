#ifndef CREGSSC61860WIDGET_H
#define CREGSSC61860WIDGET_H

#include <QWidget>

namespace Ui {
    class Cregssc61860Widget;
}

class Cregssc61860Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Cregssc61860Widget(QWidget *parent = 0);
    ~Cregssc61860Widget();

private:
    Ui::Cregssc61860Widget *ui;
};

#endif // CREGSSC61860WIDGET_H
