    #include <QApplication>
	#include <QtPlugin>
	
    #include "mainwindowpockemul.h"


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
        mainwindow->show();

        return app.exec(); 
        	
    }


