#include "cstdlib.h"

#include "mainwindowpockemul.h"
extern MainWindowPockemul *mainwindow;

Cstdlib::Cstdlib(void) {

}


void Cstdlib::setModel(QString m) {
    model = m;

    LoadLibs();
}


void Cstdlib::LoadLibs(void) {
    // Fetch all libs available for the current model
    //
    QDir dir(":/lcc/model/"+model);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            QMessageBox::about(mainwindow,"ERROR","ERROR loading standard lib :"+fileInfo.fileName());
        QByteArray line;
        while (!file.atEnd()) {
            line.append(file.readLine()+"\r\n");
        }
        libmap[fileInfo.fileName()]=line;
    }
}
