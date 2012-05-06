
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

  CcasioDOS::CcasioDOS(CPObject *parent):CcasioDisk(parent) {



  }

  CcasioDOS::~CcasioDOS() {

  }

  bool CcasioDOS::FormatDisk(void) {
      int secnum = -1;
      bool err = false;

      // CloseDiskFile(0xff);
      int maxsector = MIN(sectors,SECTORS_FAT * SIZE_SECTOR / 2);
      secbuf.fill(0x00,SIZE_SECTOR);
      for (int i = 1; i<maxsector; i++) {
          if (!SectorWrite(i)) return err;
      }
      secbuf.fill(0xff,START_DATA / SIZE_BLOCK * 2);
      if (!SectorWrite(0)) return err;
      secnum = 0;
      err = true;
      return err;
  }


  // closes all files if the specified file handle has bit 7 set }
  bool CcasioDOS::CloseDiskFile(int handle) {
      if (handle >= 0x80) {     //close all files }
          DosStatus = dsNoError;
          for (int i = 0 ; i < MAX_FILES;i++) fileinfo[i].dirindex = -1;
      }
      else {
          DosStatus = CheckFileHandle(handle);
          if (handle < MAX_FILES) fileinfo[handle].dirindex = -1;
      }
  }

CcasioDOS::TDosStatusCode CcasioDOS::CheckFileHandle(int handle) {
    if (handle >= MAX_FILES) return dsHandleInvalid;
    else if (fileinfo[handle].dirindex < 0) return dsFileNotOpened;
    else return dsNoError;
}
