#ifndef LAUNCHBUTTONWIDGET_H
#define LAUNCHBUTTONWIDGET_H

#include <QWidget>

class FluidLauncher;
class QTabWidget;

class LaunchButtonWidget:public QWidget
{
    Q_OBJECT
public:
    LaunchButtonWidget(QWidget* parent,QString,QString);

    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *event);
    QWidget* slidePanel;
    FluidLauncher* launcher;

    QString config;
    QString image;




};


#endif // LAUNCHBUTTONWIDGET_H
