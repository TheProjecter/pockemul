#include <QPainter>

#include "launchbuttonwidget.h"
#include "fluidlauncher.h"

extern QWidget* mainwidget;

LaunchButtonWidget::LaunchButtonWidget(QWidget *parent):QWidget(parent)
{

}




void LaunchButtonWidget::mouseMoveEvent(QMouseEvent *event)
{
    qWarning("hover\n");


    // create the sliding widget

    if (slidePanel) delete(slidePanel);

    slidePanel = new QWidget(mainwidget);
    slidePanel->resize(mainwidget->width(),mainwidget->height());
    slidePanel->show();
    launcher = new FluidLauncher(slidePanel);
    launcher->show();

    QPropertyAnimation *animation = new QPropertyAnimation(slidePanel, "geometry");
    animation->setDuration(500);
    animation->setStartValue(QRect(-mainwidget->width(), 0, mainwidget->width(), mainwidget->height()));
    animation->setEndValue(QRect(0, 0, mainwidget->width(), mainwidget->height()));

    animation->start();

}

void LaunchButtonWidget::paintEvent(QPaintEvent *event) {

    QPainter painter(this);
    painter.drawImage(0,0,QImage(":/POCKEMUL/pockemul/open.png").scaled(30,30));
    painter.end();

}
