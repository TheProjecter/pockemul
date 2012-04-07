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
#if 1
    slidePanel = new QWidget(mainwidget);
    slidePanel->resize(mainwidget->width(),mainwidget->height());
    slidePanel->show();
#else
    slidePanel = new QTabWidget(mainwidget);
    slidePanel->resize(mainwidget->width(),mainwidget->height());
    slidePanel->show();
#endif

    launcher = new FluidLauncher(slidePanel);
    launcher->show();

//    slidePanel->addTab(launcher,"Pockets");
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
