
#include "casiodisk.h"




  CcasioDisk::CcasioDisk(CPObject *parent) : CPObject(parent) {
    secbuf = (char *) malloc(SIZE_SECTOR*sizeof(char));
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
      qint64 nbread = diskFile.read((char*)&secbuf[0],(qint64)SIZE_SECTOR);
      err = (nbread==SIZE_SECTOR);
      return err;
  }

  bool CcasioDisk::SectorWrite(int number) {
      bool err = false;

      if (number >= sectors) return false;
      diskFile.seek(number * SIZE_SECTOR);
      qint64 nbwrite = diskFile.write(secbuf,SIZE_SECTOR);
      err = (nbwrite == SIZE_SECTOR);

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
      memset((char *)&secbuf[0] ,0x00,SIZE_SECTOR);

      for (int i = 1; i<maxsector; i++) {
          if (!SectorWrite(i)) return err;
      }

      memset((char *)&secbuf[0] ,0xff,START_DATA / SIZE_BLOCK * 2);
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
      return true;
  }

CcasioDOS::TDosStatusCode CcasioDOS::CheckFileHandle(int handle) {
    if (handle >= MAX_FILES) return dsHandleInvalid;
    else if (fileinfo[handle].dirindex < 0) return dsFileNotOpened;
    else return dsNoError;
}

// returns the number of read bytes }
int CcasioDOS::DosSecRead (int x, char *data) {
  int ret = 0;
  if (MySecRead (x)) ret = SIZE_SECTOR;
  memmove((char*)&secbuf[0], data, SIZE_SECTOR);
  return ret;
}


// returns the number of written bytes }
int CcasioDOS::DosSecWrite (int x, char *data) {
  int ret = 0;
  memmove(data, (char*)&secbuf[0], SIZE_SECTOR);
  if (MySecWrite (x)) ret = SIZE_SECTOR;
  return ret;
}


int CcasioDOS::GetDiskFileTag (int handle) {
  int ret = 0;
  if (handle < MAX_FILES) ret = fileinfo[handle].tag;
    return ret;
}

void CcasioDOS::PutDiskFileTag (int handle, int value) {
  if (handle < MAX_FILES) fileinfo[handle].tag = value;
}

// read sector 'x' to the 'secbuf', unless it already contains valid data }
bool CcasioDOS::MySecRead(int x) {
    if (x != secnum) {
        secnum = -1;
        if (!SectorRead(x)) return false;
        secnum = x;
    }
    return true;
}

//write the contents of the 'secbuf' to the sector 'x' }
bool CcasioDOS::MySecWrite(int x) {
    secnum = -1;
    if (! SectorWrite(x)) return false;
    secnum = x;
    return true;
}

bool CcasioDOS::DosInit(void) {
    bool ret = DiskOpen();
    secnum = -1;
    CloseDiskFile(0xFF);
    return ret;
}

bool CcasioDOS::DosClose(void) {
    DiskClose();
    return true;
}

// write a directory entry 'i' with data pointed to by 'ptr' }
bool CcasioDOS::WriteDirEntry(char *ptr, int i) {
  if ((i < 0) || (i >= MAX_DIR_ENTRY)) return false;
  int x = i * SIZE_DIR_ENTRY;
  int s = x / SIZE_SECTOR + START_DIR;          // sector containing the directory entry
  if (! MySecRead (s)) return false;
  memmove((char*)ptr,(char*)&secbuf[x % SIZE_SECTOR], SIZE_DIR_ENTRY);
  if (! MySecWrite (s)) return false;
  return true;
}

// read a directory entry 'i' to the memory location pointed to by 'ptr' }
CcasioDOS::TStorageProperty CcasioDOS::ReadDirEntry (TDirEntry *dir,int i) {

  if ((i < 0) || (i >= MAX_DIR_ENTRY)) return spError;
  int x = i * SIZE_DIR_ENTRY;
  int s = x % SIZE_SECTOR + START_DIR;
  if (! MySecRead (s)) return spError;
  memmove((char*)&secbuf[x%SIZE_SECTOR],(char*)dir,SIZE_DIR_ENTRY);

  if ((dir->name[0]==0) && (dir->block[0]==0) && (dir->block[1]==0))
      return spFree;
    else
      return spOccupied;
}

// seek the directory for the specified file name, file type ignored,
//  returns the index of the directory entry or -1 if not found }
int CcasioDOS::FindDirEntry (char *filename) {

    TStorageProperty x;
    int result = 0;
    do {
        x = ReadDirEntry( &direntrybuf, result);
        if ((x == spOccupied) &&
                memcmp( (char*)direntrybuf.name, filename, SIZE_FILE_NAME)) return result;
        result++;
    } while ( x != spError);
    return -1;
}




// the function returns the FAT entry associated with the specified sector,
//  or value > $FFFF in case of an error }
qint32 CcasioDOS::ReadFatEntry (qint32 x) {

    x = x / SIZE_BLOCK * 2;		// offset of the FAT entry }
  int s = x / SIZE_SECTOR + START_FAT;  //sector containing the FAT entry
  if (s >= START_FAT + SECTORS_FAT) return -1;
  if (! MySecRead (s)) return -1;
  x = x % SIZE_SECTOR;
  return ((secbuf[x] << 8) + secbuf[x+1]);
}

// the function writes the FAT entry associated with the specified sector }
bool CcasioDOS::WriteFatEntry (qint32 x, qint32 y ) {
    x = x / SIZE_BLOCK * 2;		// offset of the FAT entry }
    int s = x / SIZE_SECTOR + START_FAT;     // sector containing the FAT entry
    if (s >= START_FAT + SECTORS_FAT) return false;
    if (!MySecRead (s)) return false;
    x = x % SIZE_SECTOR;
    secbuf[x] = (y>>8) & 0xff;  //Hi(y);
    secbuf[x+1] = y & 0xff;     //Lo(y);
    if (!MySecWrite (s)) return false;
    return true;
}


// returns the file size in records or 0 in case of an error,
//  'DosStatus' isn't modified }
qint32 CcasioDOS::SizeOfDiskFile (qint32 handle) { // number of records};
    if (CheckFileHandle(handle) != dsNoError) return 0;
    // scan the FAT chain for the last record number }
    int fromrec = fileinfo[handle].lastrec;
    int fromsec = fileinfo[handle].lastsec;
    do {
        fromsec = FatNextSector(fromsec, false);
        fromrec++;
    } while (fromsec != 0);
    return fromrec;
}

// find or allocate (if allowed) the next sector in the FAT chain,
//  returns 0 if none found }
qint32 CcasioDOS::FatNextSector (qint32 x, bool allocate) {

    qint32 y ; // sector of a newly allocated block}
    qint32 entry; // FAT entry}: cardinal;

    qint32 ret = 0;
    if (x < START_DATA) {

        // new file }
        if (! allocate) return ret;
        y = FindFreeBlock();
        if (y == 0) return ret;
        entry = FB_IN_USE + FB_LAST + y / SIZE_BLOCK;
        if (! WriteFatEntry(y, entry)) return ret;
        ret = y;

    }
    else

        // existing file }
    {
        entry = ReadFatEntry (x);
        if (entry > 0xFFFF) return ret;			// no valid FAT entry }
        if ((entry & FB_IN_USE) == 0) return ret;	// FAT error, sector marked as free }
        if ((entry & FB_LAST) != 0) {
            // last block in the chain }
            if ( (x & (SIZE_BLOCK - 1)) >= ((entry & FB_SECTORS) >> 12)) {
                // end of file }
                if (!allocate) return ret;
                if ((x & (SIZE_BLOCK - 1)) < (SIZE_BLOCK - 1)) {
                    // allocate next sector in the same block }
                    if (! WriteFatEntry(x + 1, entry + 0x1000)) return ret;
                    ret = x + 1;
                }
                else
                    // allocate next sector in a new block }
                {
                    y = FindFreeBlock();
                    if (y == 0) return ret;
                    entry = FB_IN_USE + y / SIZE_BLOCK;
                    if (! WriteFatEntry(x, entry)) return ret;	// previous in chain }
                    entry = FB_IN_USE + FB_LAST + y / SIZE_BLOCK;
                    if (!WriteFatEntry(y, entry)) return ret;	// last in chain }
                    ret = y;
                }
            }
            else
                // not the end of file }
            {
                ret = x + 1;
            }
        }
        else

            // not the last block in the chain }
        {
            if ((x & (SIZE_BLOCK - 1)) < (SIZE_BLOCK - 1)) {
                // next sector is in the same block }

                ret = x + 1;
            }
            else
                // next sector is in another block }
            {
                x = (entry & FB_BLOCK) * SIZE_BLOCK;	// follow the FAT chain }
                entry = ReadFatEntry (x);
                if (entry > 0xFFFF) return ret;		// no valid FAT entry }
                if ((entry & FB_IN_USE) != 0) ret= x;
            }
        }

    }
    return ret;
}

// find first free block,
//  returns the number of first sector of the block, or 0 if none found }
qint32 CcasioDOS::FindFreeBlock(void) {
    qint32 maxsector = MIN(sectors, SECTORS_FAT * SIZE_SECTOR / 2);
    qint32 ret = START_DATA;
    do {
        if ((ReadFatEntry(ret) & FB_IN_USE) == 0) return ret;
        ret += SIZE_BLOCK;
    } while (ret < maxsector);
    ret = 0;
    return ret;
}

