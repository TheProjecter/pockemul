

#include <QApplication>
#include <QtPlugin>
#include <QDebug>
#include <QDir>
#include <QSplashScreen>
//#include <QAndroidStyle>
//#include <QFeedbackHapticsEffect>
#include "launchbuttonwidget.h"
#include "mainwindowpockemul.h"


#include "pobject.h"
#include "dialogstartup.h"
#include "init.h"

#include "downloadmanager.h"

//QTM_USE_NAMESPACE

#include "version.h"



MainWindowPockemul* mainwindow;
DownloadManager* downloadManager;



int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    QCoreApplication::setOrganizationDomain("pockemul.free.fr");
    QCoreApplication::setOrganizationName("Remy Corp.");
    QCoreApplication::setApplicationName("PockEmul ");
    QCoreApplication::setApplicationVersion(POCKEMUL_VERSION);

#ifdef Q_OS_MAC
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("plugins");
    QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
    qWarning()<<QApplication::libraryPaths();
#endif

    QDir home = QDir::home();
    if (!home.exists("pockemul")) {
        home.mkpath("pockemul/documents");
    }
#if 0

    QFeedbackHapticsEffect rumble;
    rumble.setAttackIntensity(0.0);
    rumble.setAttackTime(250);
    rumble.setIntensity(1.0);
    rumble.setDuration(500);
    rumble.setFadeTime(250);
    rumble.setFadeIntensity(0.0);

    rumble.start();
#endif



#ifdef Q_OS_ANDROID
//    QFont f = app.font();
//    f.setItalic(true); //bold also works
//    app.setFont(f);

    // Change currentPath to /sdcard/pockemul
    QDir d("/");
    d.mkdir("/sdcard/pockemul");
    QDir::setCurrent("/sdcard/pockemul");

    QWindowsStyle *s = new QWindowsStyle();//QAndroidStyle();

    QApplication::setStyle(s);//new QAndroidStyle());

#endif

    mainwindow = new MainWindowPockemul;

//    qWarning()<<app.topLevelWidgets();

#ifdef Q_OS_ANDROID
    QSplashScreen splash;
    splash.setPixmap(QPixmap(":/POCKEMUL/pockemul/splash.png").scaled(mainwindow->geometry().size()));
    splash.showFullScreen();
    splash.showMessage("Loading modules...",Qt::AlignLeft,Qt::white);
    app.processEvents();
    splash.finish(mainwindow);

    mainwindow->menuBar()->setVisible(false);//->menuAction()->setVisible( false );
#endif

    QWidget *cw= new QWidget();
    mainwindow->setCentralWidget(cw);
    delete mainwindow->centralwidget;

    mainwindow->centralwidget = cw;
    mainwindow->setWindowIcon ( QIcon(":/POCKEMUL/pockemul/pockemul.bmp") );
    mainwindow->resize(680,505);


    LaunchButtonWidget* launch1 = new LaunchButtonWidget(mainwindow->centralwidget,
                                                LaunchButtonWidget::PictureFlow,
                                                ":/POCKEMUL/pockemul/config.xml",
                                                ":/POCKEMUL/pockemul/pocket.png");
    launch1->setGeometry(0,12,48,48);
    launch1->setToolTip("Start a new Pocket Emulation.");

    LaunchButtonWidget* launch2 = new LaunchButtonWidget(mainwindow->centralwidget,
                                                 LaunchButtonWidget::PictureFlow,
                                                 ":/POCKEMUL/pockemul/configExt.xml",
                                                 ":/POCKEMUL/pockemul/ext.png");
    launch2->setGeometry(0,75,48,48);
    launch2->setToolTip("Start a new Extension Emulation.");

#ifndef Q_OS_ANDROID
    LaunchButtonWidget* dev = new LaunchButtonWidget(mainwindow->centralwidget,
                                                     LaunchButtonWidget::Action,
                                                     "",
                                                     ":/POCKEMUL/pockemul/dev.png");
    mainwindow->connect(dev,SIGNAL(clicked()),mainwindow,SLOT(IDE()));
    dev->setGeometry(0,150,48,48);
    dev->setToolTip("Start the Integrated development Environment.");
#endif
    LaunchButtonWidget* save = new LaunchButtonWidget(mainwindow->centralwidget,
                                                      LaunchButtonWidget::Action,
                                                      "",
                                                      ":/POCKEMUL/pockemul/save.png");
    mainwindow->connect(save,SIGNAL(clicked()),mainwindow,SLOT(saveassession()));
    save->setGeometry(0,225,48,48);
    save->setToolTip("Save the current session.");

#if 0
    LaunchButtonWidget* load = new LaunchButtonWidget(mainwindow->centralwidget,
                                                      LaunchButtonWidget::Action,
                                                      "",
                                                      ":/POCKEMUL/pockemul/load.png");
    mainwindow->connect(load,SIGNAL(clicked()),mainwindow,SLOT(opensession()));
    load->setGeometry(0,300,48,48);
    load->setToolTip("Load an existing session.");
#else
    LaunchButtonWidget* load = new LaunchButtonWidget(mainwindow->centralwidget,
                                                      LaunchButtonWidget::FileBrowser,
                                                      ".",
                                                      ":/POCKEMUL/pockemul/load.png");
//    mainwindow->connect(load,SIGNAL(clicked()),mainwindow,SLOT(opensession()));
    load->setGeometry(0,300,48,48);
    load->setToolTip("Load an existing session.");

#endif

    LaunchButtonWidget* exit = new LaunchButtonWidget(mainwindow->centralwidget,
                                                      LaunchButtonWidget::Action,
                                                      "",
                                                      ":/POCKEMUL/pockemul/exit.png");
    mainwindow->connect(exit,SIGNAL(clicked()),qApp, SLOT(quit()));//closeAllWindows()));
    exit->setGeometry(0,375,48,48);
    exit->setToolTip("Exit PockEmul.");

//    CTinyBasic tb;
//    tb.test();

    downloadManager = new DownloadManager();
    downloadManager->targetDir = QDir::homePath()+"/pockemul/documents";

    mainwindow->show();

#ifndef Q_OS_ANDROID
    mainwindow->initCommandLine();
#endif

//    mainwindow->LoadPocket(PC1211);

    return app.exec();

}


