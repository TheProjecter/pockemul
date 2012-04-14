#include <QPainter>

#include "launchbuttonwidget.h"
#include "fluidlauncher.h"

extern QWidget* mainwidget;




LaunchButtonWidget::LaunchButtonWidget(QWidget *parent,LaunchButtonWidget::LaunchType type,QString param,QString img):QWidget(parent)
{
    this->type = type;
    config = param;
    image = img;
    slidePanel = 0;
    setAttribute(Qt::WA_DeleteOnClose);
}




void LaunchButtonWidget::mousePressEvent(QMouseEvent *event)
{
    qWarning("hover : %s  %i\n",image.toAscii().data(),slidePanel);


    emit clicked();

    if (type == PictureFlow)
    {

        // create the sliding widget

        if (slidePanel) slidePanel->close();

        slidePanel = new QWidget(mainwidget);
        slidePanel->resize(mainwidget->width(),mainwidget->height());
        slidePanel->show();

        launcher = new FluidLauncher(slidePanel,config);
        launcher->show();
        qWarning("hover2\n");
        QPropertyAnimation *animation = new QPropertyAnimation(slidePanel, "geometry");
        animation->setDuration(500);
        animation->setStartValue(QRect(-mainwidget->width(), 0, mainwidget->width(), mainwidget->height()));
        animation->setEndValue(QRect(0, 0, mainwidget->width(), mainwidget->height()));

        animation->start();
        qWarning("hover3\n");
    }
    event->accept();
}

void LaunchButtonWidget::paintEvent(QPaintEvent *event) {

    QPainter painter(this);
    painter.drawImage(0,0,QImage(image).scaled(size()));
    painter.end();

}
