#ifndef LAUNCHBUTTONWIDGET_H
#define LAUNCHBUTTONWIDGET_H

#include <QWidget>

class LaunchButtonWidget:public QWidget
{
public:
    LaunchButtonWidget(QWidget* parent);

    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *event);
};


#endif // LAUNCHBUTTONWIDGET_H
