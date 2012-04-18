#include <QApplication>
#include <QtPlugin>
#include <QDebug>
#include <QDir>

#include "mainwindowpockemul.h"
#include "launchbuttonwidget.h"

#include "pobject.h"
#include "dialogstartup.h"


MainWindowPockemul* mainwindow;
QWidget* mainwidget;

//	Q_IMPORT_PLUGIN(qjpeg)

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);






#ifdef Q_OS_ANDROID
    QFont f = app.font();
    f.setItalic(true); //bold also works
    app.setFont(f);

    // Change currentPath to /sdcard/pockemul
    QDir d("/");
    d.mkdir("/sdcard/pockemul");
    QDir::setCurrent("/sdcard/pockemul");

#endif

    mainwindow = new MainWindowPockemul;

#ifdef Q_OS_ANDROID
    //mainwindow->menuBar()->setVisible(false);//->menuAction()->setVisible( false );
#endif

    QWidget *cw= new QWidget();
    mainwindow->setCentralWidget(cw);

    mainwidget = mainwindow->centralWidget();
    mainwindow->setWindowIcon ( QIcon(":/POCKEMUL/pockemul/pockemul.bmp") );
    mainwindow->resize(680,505);



    mainwindow->launch = new LaunchButtonWidget(mainwidget,
                                                LaunchButtonWidget::PictureFlow,
                                                ":/POCKEMUL/pockemul/config.xml",
                                                ":/POCKEMUL/pockemul/pocket.png");
    mainwindow->launch->setMouseTracking(true);
    mainwindow->launch->setGeometry(0,0,48,48);
    mainwindow->launch->setToolTip("Start a new Pocket Emulation.");

    mainwindow->launch2 = new LaunchButtonWidget(mainwidget,
                                                 LaunchButtonWidget::PictureFlow,
                                                 ":/POCKEMUL/pockemul/configExt.xml",
                                                 ":/POCKEMUL/pockemul/ext.png");
    mainwindow->launch2->setMouseTracking(true);
    mainwindow->launch2->setGeometry(0,50,48,48);
    mainwindow->launch2->setToolTip("Start a new Extension Emulation.");

#ifndef Q_OS_ANDROID
    LaunchButtonWidget* dev = new LaunchButtonWidget(mainwidget,
                                                     LaunchButtonWidget::Action,
                                                     "",
                                                     ":/POCKEMUL/pockemul/dev.png");
    mainwindow->connect(dev,SIGNAL(clicked()),mainwindow,SLOT(IDE()));
    dev->setGeometry(0,100,48,48);
    dev->setToolTip("Start the Integrated development Environment.");
#endif
    LaunchButtonWidget* save = new LaunchButtonWidget(mainwidget,
                                                      LaunchButtonWidget::Action,
                                                      "",
                                                      ":/POCKEMUL/pockemul/save.png");
    mainwindow->connect(save,SIGNAL(clicked()),mainwindow,SLOT(saveassession()));
    save->setGeometry(0,150,48,48);
    save->setToolTip("Save the current session.");

    LaunchButtonWidget* load = new LaunchButtonWidget(mainwidget,
                                                      LaunchButtonWidget::Action,
                                                      "",
                                                      ":/POCKEMUL/pockemul/load.png");
    mainwindow->connect(load,SIGNAL(clicked()),mainwindow,SLOT(opensession()));
    load->setGeometry(0,200,48,48);
    load->setToolTip("Load an existing session.");

    LaunchButtonWidget* exit = new LaunchButtonWidget(mainwidget,
                                                      LaunchButtonWidget::Action,
                                                      "",
                                                      ":/POCKEMUL/pockemul/exit.png");
    mainwindow->connect(exit,SIGNAL(clicked()),mainwindow,SLOT(close()));
    exit->setGeometry(0,250,48,48);
    exit->setToolTip("Exit PockEmul.");

#if 0 //def Q_OS_ANDROID
    DialogStartup dialogstartup(0);
    int result = dialogstartup.exec();
    CPObject * pPC = mainwindow->LoadPocket(result);
    pPC->setParent(0,Qt::FramelessWindowHint);
    pPC->show();
#else
    mainwindow->show();

#endif
    return app.exec();

}


