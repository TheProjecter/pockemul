#ifndef CASIODISK_H
#define CASIODISK_H

#include <QFile>

#include "pobject.h"


#define SIZE_SECTOR 256



class CcasioDisk: public CPObject
{
    Q_OBJECT

    CcasioDisk(CPObject *parent = 0);
    ~CcasioDisk();
    bool SectorWrite(int number);
    bool SectorRead(int number);
    bool DiskClose();
    bool DiskOpen();


private:
    QByteArray secbuf;
    int sectors;	// available number of sectors
    QFile diskFile;
};




#endif // CASIODISK_H
