

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

#ifdef P_AVOID
#include "libavoid.h"
#endif

#include "downloadmanager.h"

//QTM_USE_NAMESPACE

#include "version.h"



MainWindowPockemul* mainwindow;
DownloadManager* downloadManager;

QString appDir;

int main(int argc, char *argv[])
{

    QApplication *app = new QApplication(argc, argv);
     app->setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

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

//    QProxyStyle *s = new QProxyStyle();//QAndroidStyle();

//    QApplication::setStyle(s);//new QAndroidStyle());

#endif

    mainwindow = new MainWindowPockemul;

    appDir = app->applicationDirPath();

    qWarning()<<app->applicationDirPath();

#ifdef Q_OS_ANDROID
//    QSplashScreen splash;
//    splash.setPixmap(QPixmap(P_RES(":/pockemul/splash.png")).scaled(mainwindow->geometry().size()));
//    splash.showFullScreen();
//    splash.showMessage("Loading modules...",Qt::AlignLeft,Qt::white);
//    app->processEvents();
//    splash.finish(mainwindow);

    mainwindow->menuBar()->setVisible(false);//->menuAction()->setVisible( false );
#endif

    QWidget *cw= new QWidget();
    mainwindow->setCentralWidget(cw);
    delete mainwindow->centralwidget;

    mainwindow->centralwidget = cw;
    mainwindow->setWindowIcon ( QIcon(":/core/pockemul.bmp") );
    mainwindow->resize(680,520);

#ifdef Q_OS_ANDROID
    cw->setStyleSheet("background-color:black;color: white;selection-background-color: grey;");
#endif

#ifdef EMSCRIPTEN
    mainwindow->setWindowTitle("PockEmul Online");
#endif
    int v_inter = 60;
    int v_pos = 12;
    LaunchButtonWidget* launch1 = new LaunchButtonWidget(mainwindow->centralwidget,
                                                LaunchButtonWidget::PictureFlow,
                                                QStringList()<<P_RES(":/pockemul/config.xml"),
                                                ":/core/pocket.png");
    launch1->setGeometry(0,v_pos,48,48);
    v_pos += v_inter;
    launch1->setToolTip("Start a new Pocket Emulation.");

#ifndef EMSCRIPTEN
    LaunchButtonWidget* launch2 = new LaunchButtonWidget(mainwindow->centralwidget,
                                                 LaunchButtonWidget::PictureFlow,
                                                 QStringList()<<P_RES(":/pockemul/configExt.xml"),
                                                 ":/core/ext.png");
    launch2->setGeometry(0,v_pos,48,48);
    v_pos += v_inter;
    launch2->setToolTip("Start a new Extension Emulation.");
#endif

#ifdef P_IDE
    LaunchButtonWidget* dev = new LaunchButtonWidget(mainwindow->centralwidget,
                                                     LaunchButtonWidget::Action,
                                                     QStringList(),
                                                     ":/core/dev.png");
    mainwindow->connect(dev,SIGNAL(clicked()),mainwindow,SLOT(IDE()));
    dev->setGeometry(0,v_pos,48,48);
    v_pos += v_inter;
    dev->setToolTip("Start the Integrated development Environment.");
#endif
    LaunchButtonWidget* save = new LaunchButtonWidget(mainwindow->centralwidget,
                                                      LaunchButtonWidget::Action,
                                                      QStringList(),
                                                      ":/core/save.png");
    mainwindow->connect(save,SIGNAL(clicked()),mainwindow,SLOT(saveassession()));
    save->setGeometry(0,v_pos,48,48);
    v_pos += v_inter;
    save->setToolTip("Save the current session.");

    LaunchButtonWidget* load = new LaunchButtonWidget(mainwindow->centralwidget,
                                                      LaunchButtonWidget::FileBrowser,
                                                      QStringList()<<"."<<"*.pml",
                                                      ":/core/load.png");
//    mainwindow->connect(load,SIGNAL(clicked()),mainwindow,SLOT(opensession()));
    load->setGeometry(0,v_pos,48,48);
    v_pos += v_inter;
    load->setToolTip("Load an existing session.");

    LaunchButtonWidget* bookcase = new LaunchButtonWidget(mainwindow->centralwidget,
                                                      LaunchButtonWidget::FileBrowser,
                                                          QStringList()<< (QDir::homePath()+"/pockemul/documents")<<"*.pdf",
                                                      ":/core/bookcase.png");
//    mainwindow->connect(load,SIGNAL(clicked()),mainwindow,SLOT(opensession()));
    bookcase->setGeometry(0,v_pos,48,48);
    v_pos += v_inter;
    bookcase->setToolTip("Browse the bookcase.");


    LaunchButtonWidget* exit = new LaunchButtonWidget(mainwindow->centralwidget,
                                                      LaunchButtonWidget::Action,
                                                      QStringList(),
                                                      ":/core/exit.png");
    mainwindow->connect(exit,SIGNAL(clicked()),qApp, SLOT(quit()));//closeAllWindows()));
    exit->setGeometry(0,v_pos,48,48);
    v_pos += v_inter;
    exit->setToolTip("Exit PockEmul.");

//    CTinyBasic tb;
//    tb.test();
#ifndef EMSCRIPTEN
    downloadManager = new DownloadManager();
    downloadManager->targetDir = QDir::homePath()+"/pockemul/documents";
#endif

#ifdef EMSCRIPTEN
    mainwindow->zoomSlider = new QSlider(mainwindow->centralwidget);
    mainwindow->zoomSlider->setMinimum(10);
    mainwindow->zoomSlider->setMaximum(300);
    mainwindow->zoomSlider->setTickInterval(10);
    mainwindow->zoomSlider->setValue(100);
#endif

    mainwindow->show();


#ifndef Q_OS_ANDROID
    mainwindow->initCommandLine();
#endif

//    mainwindow->LoadPocket(PC1211);
#ifdef EMSCRIPTEN
    app->exec();
    return 0;
#endif

    return app->exec();


}


