#ifndef LAUNCHBUTTONWIDGET_H
#define LAUNCHBUTTONWIDGET_H

#include <QWidget>

class FluidLauncher;
class QTabWidget;

class LaunchButtonWidget:public QWidget
{
    Q_OBJECT
public:
    LaunchButtonWidget(QWidget* parent);

    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *event);
    QWidget* slidePanel;
    FluidLauncher* launcher;




};


#endif // LAUNCHBUTTONWIDGET_H
