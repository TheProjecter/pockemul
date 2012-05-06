#include <QPainter>

#include "launchbuttonwidget.h"
#include "fluidlauncher.h"
#include "mainwindowpockemul.h"

extern MainWindowPockemul* mainwindow;




LaunchButtonWidget::LaunchButtonWidget(QWidget *parent,LaunchButtonWidget::LaunchType type,QString param,QString img):QWidget(parent)
{
    this->type = type;
    config = param;
    image = img;
    setAttribute(Qt::WA_DeleteOnClose);
    if (type == PictureFlow){
        launcher = new FluidLauncher(mainwindow,config);
        launcher->hide();
    }
}




void LaunchButtonWidget::mousePressEvent(QMouseEvent *event)
{

    emit clicked();

    if (type == PictureFlow)
    {
//        launcher = new FluidLauncher(mainwindow,config);
//        launcher->setAttribute(Qt::WA_DeleteOnClose);
        launcher->setGeometry(QRect(-mainwindow->width(), 0, mainwindow->width(), mainwindow->height()));
        launcher->raise();
        launcher->show();
        launcher->currentWidget()->setFocus();
//        qWarning("hover2\n");
        QPropertyAnimation *animation = new QPropertyAnimation(launcher, "geometry");
        animation->setDuration(500);
        animation->setStartValue(QRect(-mainwindow->width(), 0, mainwindow->width(), mainwindow->height()));
        animation->setEndValue(QRect(0, 0, mainwindow->width(), mainwindow->height()));
        animation->start();
//        qWarning("hover3\n");
    }
    event->accept();
}

void LaunchButtonWidget::paintEvent(QPaintEvent *event) {

    QPainter painter(this);
    painter.drawImage(0,0,QImage(image).scaled(size()));
    painter.end();

}
