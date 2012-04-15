#ifndef LAUNCHBUTTONWIDGET_H
#define LAUNCHBUTTONWIDGET_H

#include <QWidget>

class FluidLauncher;
class QTabWidget;




class LaunchButtonWidget:public QWidget
{
    Q_OBJECT

public:

    enum LaunchType
    {
        PictureFlow,
        Action
    };


    LaunchButtonWidget(QWidget* parent,LaunchType type,QString param,QString img);

    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *event);
    FluidLauncher* launcher;

    QString config;
    QString image;
    LaunchType type;

signals:
    void clicked();


};


#endif // LAUNCHBUTTONWIDGET_H
