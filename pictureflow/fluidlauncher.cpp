/****************************************************************************
 **
 ** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the demonstration applications of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** GNU Lesser General Public License Usage
 ** This file may be used under the terms of the GNU Lesser General Public
 ** License version 2.1 as published by the Free Software Foundation and
 ** appearing in the file LICENSE.LGPL included in the packaging of this
 ** file. Please review the following information to ensure the GNU Lesser
 ** General Public License version 2.1 requirements will be met:
 ** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights. These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU General
 ** Public License version 3.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of this
 ** file. Please review the following information to ensure the GNU General
 ** Public License version 3.0 requirements will be met:
 ** http://www.gnu.org/copyleft/gpl.html.
 **
 ** Other Usage
 ** Alternatively, this file may be used in accordance with the terms and
 ** conditions contained in a signed written agreement between you and Nokia.
 **
 **
 **
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

 #include <QXmlStreamReader>

 #include "fluidlauncher.h"
#include "init.h"
#include "mainwindowpockemul.h"

extern MainWindowPockemul *mainwindow;

 #define DEFAULT_INPUT_TIMEOUT 10000
 #define SIZING_FACTOR_HEIGHT 6/10
 #define SIZING_FACTOR_WIDTH 6/10


FluidLauncher::FluidLauncher(QWidget * parent,QString config):QStackedWidget(parent)
{
//    qWarning("CFL 1\n");

    pictureFlowWidget = new PictureFlow();

    addWidget(pictureFlowWidget);

    setCurrentWidget(pictureFlowWidget);
    pictureFlowWidget->setFocus();

    QRect screen_size = parent->geometry();//QApplication::desktop()->screenGeometry();
    resize(parent->size());

//    qWarning()<<screen_size;
//    qWarning()<<mainwindow->centralwidget->geometry();

    QObject::connect(pictureFlowWidget, SIGNAL(itemActivated(int)), this, SLOT(launchApplication(int)));

    const int h = screen_size.height() * SIZING_FACTOR_HEIGHT;
    const int w = screen_size.width() * SIZING_FACTOR_WIDTH;

    const int hh = qMin(h, w);
    const int ww = hh / 3 * 4;
    pictureFlowWidget->setSlideSize(QSize(ww, hh));

    bool success;
//    qWarning("CFL 2\n");
    success = loadConfig(config);
//    qWarning("CFL 3\n");
    if (success) {
        populatePictureFlow();
//        qWarning("CFL 4\n");
//        show();

    } else {

        pictureFlowWidget->close();
    }
//    qWarning("CFL 5\n");

}

 FluidLauncher::~FluidLauncher()
 {
//     qWarning("Delete pictureFlowWidget");
     delete pictureFlowWidget;
     // Delete demolist
     for (int i=0;i<demoList.count();i++) {
         delete demoList.at(i);
     }
 }

 bool FluidLauncher::loadConfig(QString configPath)
 {
//     qWarning("loadConfig \n");
     QFile xmlFile(configPath);

     if (!xmlFile.exists() || (xmlFile.error() != QFile::NoError)) {
         qDebug() << "ERROR: Unable to open config file " << configPath;
         return false;
     }

     xmlFile.open(QIODevice::ReadOnly);
     QXmlStreamReader reader(&xmlFile);
     while (!reader.atEnd()) {
         reader.readNext();

         if (reader.isStartElement()) {
             if (reader.name() == "demos")
                 parseDemos(reader);
         }
     }

     if (reader.hasError()) {
        qDebug() << QString("Error parsing %1 on line %2 column %3: \n%4")
                 .arg(configPath)
                 .arg(reader.lineNumber())
                 .arg(reader.columnNumber())
                 .arg(reader.errorString());
     }

     // Append an exit Item
//     Launcher* exitItem = new Launcher("EXIT",QString(), QLatin1String("Exit"), ":/cover/exit.png", QStringList());
//     demoList.append(exitItem);

//     qWarning("nb slide:%i   %i\n",pictureFlowWidget->slideCount(),demoList.count());
     return true;
 }

 void FluidLauncher::parseDemos(QXmlStreamReader& reader)
 {
     while (!reader.atEnd()) {
         reader.readNext();
         if (reader.isStartElement() && reader.name() == "example") {
             QXmlStreamAttributes attrs = reader.attributes();
             QStringRef filename = attrs.value("filename");
             if (!filename.isEmpty()) {
                 QStringRef name = attrs.value("name");
                 QStringRef image = attrs.value("image");
                 QStringRef args = attrs.value("args");
                 QStringRef idpocket = attrs.value("idpocket");

                 Launcher* newDemo = new Launcher(
                         idpocket.toString(),
                         filename.toString(),
                         name.isEmpty() ? "Unnamed Demo" : name.toString(),
                         image.toString(),
                         args.toString().split(" "));
                 demoList.append(newDemo);
             }
         } else if(reader.isEndElement() && reader.name() == "demos") {
             return;
         }
     }
 }


 void FluidLauncher::populatePictureFlow()
 {

     pictureFlowWidget->setSlideCount(demoList.count());

     for (int i=demoList.count()-1; i>=0; --i) {
         QImage *img = demoList[i]->getImage();
         pictureFlowWidget->setSlide(i, *img);
         pictureFlowWidget->setSlideCaption(i, demoList[i]->getCaption());
         delete img;
     }

     pictureFlowWidget->setCurrentSlide(demoList.count()/2);

 }

 void FluidLauncher::launchApplication(int index)
 {


     // NOTE: Clearing the caches will free up more memory for the demo but will cause
     // a delay upon returning, as items are reloaded.
     //pictureFlowWidget->clearCaches();

     if ( (index==-1) )//||(index == demoList.size() -1))
     {
         hide();
         return;
     }

#if 0
     QObject::connect(demoList[index], SIGNAL(demoFinished()), this, SLOT(demoFinished()));

     demoList[index]->launch();
#endif

     int result = EMPTY;
     QString ItemText = demoList[index]->getIdPocket();

     //TODO: try to implement sub level
     // if ItemText start with a specic tag, load the corresponding xml and display the new pictureflow.
     // Esc should exit

     if (ItemText == "PC 1245")	result = PC1245;
     if (ItemText == "MC 2200")	result = MC2200;
     if (ItemText == "PC 1250")	result = PC1250;
     if (ItemText == "PC 1251")	result = PC1251;
     if (ItemText == "PC 1255")	result = PC1255;
     if (ItemText == "Tandy PC-3")	result = TandyPC3;
     if (ItemText == "Tandy PC-3 (4Ko)")	result = TandyPC3EXT;
     if (ItemText == "PC 1260")	result = PC1260;
     if (ItemText == "PC 1261")	result = PC1261;
     if (ItemText == "PC 1262")	result = PC1262;
     if (ItemText == "PC 1280")	result = PC1280;
     if (ItemText == "PC 1350")	result = PC1350;
     if (ItemText == "PC 1360")	result = PC1360;
     if (ItemText == "PC 1401")	result = PC1401;
     if (ItemText == "PC 1402")	result = PC1402;
     if (ItemText == "PC 1403")	result = PC1403;
     if (ItemText == "PC 1421")	result = PC1421;
     if (ItemText == "PC 1403H")	result = PC1403H;
     if (ItemText == "PC 1450")	result = PC1450;
     if (ItemText == "PC 1475")	result = PC1475;
     if (ItemText == "PC 1500")	result = PC1500;
     if (ItemText == "PC 1500A")	result = PC1500A;
     if (ItemText == "Tandy PC-2")	result = TandyPC2;
     if (ItemText == "PC 1251H")	result = PC1251H;
     if (ItemText == "PC 1600")	result = PC1600;
     if (ItemText == "PC 2500")	result = PC2500;

     if (ItemText == "CE-125")	result = CE125;
     if (ItemText == "MP-220")	result = MP220;
     if (ItemText == "CE-120P")	result = CE120P;
     if (ItemText == "CE-126P")	result = CE126P;
     if (ItemText == "CE-123P")	result = CE123P;
     if (ItemText == "CE-129P")	result = CE129P;
     if (ItemText == "CE-140P")	result = CE140P;
     if (ItemText == "CE-140F")	result = CE140F;
     if (ItemText == "CE-150")	result = CE150;
     if (ItemText == "CE-152")	result = CE152;
     if (ItemText == "26-3591")	result = TANDY263591;

     if (ItemText == "Serial Console")	result = SerialConsole;
     if (ItemText == "11Pins Cable")	result = CABLE11Pins;
     if (ItemText == "Potar")	result = POTAR;
     if (ItemText == "Simulator")	result = Simulator;

     if (ItemText == "CE-1600P")	result = CE1600P;
     if (ItemText == "Canon X-07") result = X07;
     if (ItemText == "Canon X-710") result = X710;
     if (ItemText == "PC-E500") result = E500;
     if (ItemText == "PC-E550") result = E550;
     if (ItemText == "PC-G850V") result = G850V;
     if (ItemText == "Casio PB-1000") result = PB1000;
     if (ItemText == "Casio PB-2000C") result = PB2000;
     if (ItemText == "Casio MD-100") result = MD100;
     if (ItemText == "Casio FP-100") result = FP100;
     if (ItemText == "Casio FP-200") result = FP200;
     if (ItemText == "Casio Z-1GR") result = Z1GR;

     if (result != EMPTY)	{
         mainwindow->LoadPocket(result);
         close();
//         parentWidget()->close();
     }



 }

 void FluidLauncher::exitSlot()
 {
//     parentWidget()->close();
     close();
 }

 void FluidLauncher::demoFinished()
 {
     setCurrentWidget(pictureFlowWidget);

     // Bring the Fluidlauncher to the foreground to allow selecting another demo
     raise();
     activateWindow();
 }


