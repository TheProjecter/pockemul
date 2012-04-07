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
    slidePanel->resize(mainwidget->width(),150);
    slidePanel->show();
    launcher = new FluidLauncher(slidePanel);
    launcher->show();

//    connect(launcher,SIGNAL(launchPocket()),this,SLOT(launch()));

    QPropertyAnimation *animation = new QPropertyAnimation(slidePanel, "geometry");
    animation->setDuration(500);
    animation->setStartValue(QRect(-mainwidget->width(), 0, mainwidget->width(), 150));
    animation->setEndValue(QRect(0, 0, mainwidget->width(), 150));

    animation->start();

}

void LaunchButtonWidget::paintEvent(QPaintEvent *event) {
//    qWarning("launch\n");

//    QPixmap pixmap(":/cover/exit.png");
////    resize(pixmap.size());
//    QPalette p;
//    p.setBrush(launch->backgroundRole(), QBrush(pixmap));
//    launch->setPalette(p);

//    launch->update();

    QPainter painter(this);
//    painter.setPen(Qt::blue);
//    painter.setFont(QFont("Arial", 30));
//    painter.drawText(rect(), Qt::AlignCenter, "Qt");


painter.drawImage(0,0,QImage(":/POCKEMUL/pockemul/open.png").scaled(30,30));

painter.end();
//launch->update();
}
