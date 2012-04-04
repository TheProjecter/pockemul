#include <QPainter>

#include "launchbuttonwidget.h"
#include "fluidlauncher.h"


LaunchButtonWidget::LaunchButtonWidget(QWidget *parent):QWidget(parent)
{

//        QPixmap pixmap(":/POCKEMUL/pockemul/open.png");
//        resize(pixmap.size());
//        QPalette p;
//        p.setBrush(backgroundRole(), QBrush(pixmap));
//        setPalette(p);
}

extern QWidget* mainwidget;
void LaunchButtonWidget::mouseMoveEvent(QMouseEvent *event)
{
    FluidLauncher* launcher;
    launcher = new FluidLauncher(mainwidget);

    launcher->show();
}

void LaunchButtonWidget::paintEvent(QPaintEvent *event) {
    qWarning("launch\n");

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
