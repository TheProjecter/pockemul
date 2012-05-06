
#include "casiodisk.h"



  CcasioDisk::CcasioDisk(CPObject *parent) : CPObject(parent) {

  }

  CcasioDisk::~CcasioDisk() {

  }

  bool CcasioDisk::DiskOpen(void) {
      sectors = -1;
      diskFile.setFileName("disk0.bin");
      diskFile.open(QIODevice::ReadWrite);
      sectors = diskFile.size() / SIZE_SECTOR;

      return (sectors >= 0);
  }

  bool CcasioDisk::DiskClose(void) {
      diskFile.close();
      sectors = -1;
      return true;
  }

  bool CcasioDisk::SectorRead(int number) {
      bool err = false;
      if (number >= sectors) return false;
      diskFile.seek(number * SIZE_SECTOR);
      secbuf = diskFile.read((qint64)SIZE_SECTOR);
      err = (secbuf.size()==SIZE_SECTOR);
      return err;
  }

  bool CcasioDisk::SectorWrite(int number) {
      bool err = false;

      if (secbuf.size() != SIZE_SECTOR ) return false;
      if (number >= sectors) return false;
      diskFile.seek(number * SIZE_SECTOR);
      qint64 nbwrite = diskFile.write(secbuf);
      err = (nbwrite == -1);

      return err;
  }


