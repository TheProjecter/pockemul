    #include <QApplication>
	#include <QtPlugin>
	
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
#endif

        mainwindow = new MainWindowPockemul;


        QWidget *cw= new QWidget();
        mainwindow->setCentralWidget(cw);

        mainwidget = mainwindow->centralWidget();
		mainwindow->setWindowIcon ( QIcon(":/POCKEMUL/pockemul/pockemul.bmp") );      
        mainwindow->resize(680,505);



        mainwindow->launch = new LaunchButtonWidget(mainwidget);
        mainwindow->launch->setMouseTracking(true);
        mainwindow->launch->setGeometry(0,0,30,30);
//        mainwindow->launch->show();
//        launch->raise();
//        launch->update();


#ifdef Q_OS_ANDROID
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


