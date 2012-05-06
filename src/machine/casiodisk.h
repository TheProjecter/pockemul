#ifndef CASIODISK_H
#define CASIODISK_H

#include <QFile>

#include "pobject.h"

#define SIZE_SECTOR     256
#define SIZE_DIR_ENTRY  16

#define SECTORS_FAT     4
#define SECTORS_DIR     12
#define START_FAT       0
#define START_DIR       4
#define START_DATA      16
#define MAX_DIR_ENTRY   (SECTORS_DIR * SIZE_SECTOR / SIZE_DIR_ENTRY)
#define MAX_FILES       16			// number of handles }
#define SIZE_FILE_NAME  (8+3)
#define SIZE_BLOCK      4			// 4 sectors per block }

class CcasioDisk: public CPObject
{
    Q_OBJECT
public:
    CcasioDisk(CPObject *parent = 0);
    ~CcasioDisk();


protected:
    bool SectorWrite(int number);
    bool SectorRead(int number);
    bool DiskClose();
    bool DiskOpen();

    QByteArray secbuf;
    int sectors;	// available number of sectors
    QFile diskFile;
};

// Structure of directory entry }
typedef struct {
      BYTE kind;			// file type }
      BYTE name[8];         //: array [0..7] of byte;
      BYTE ext[3];          //: array [0..2] of byte;
      BYTE unknown;			// unknown meaning, probably unused }
      BYTE block[2];        // starting block number, MSB first }
      BYTE unused;			// always zero }
    } TDirEntry;




class CcasioDOS:public CcasioDisk {

public:

    enum TDosStatusCode {
        dsNoError,
        dsRenameFailed,		// file of specified new name already exists }
        dsFileNotFound,		// open, delete, rename }
        dsFileNotOpened,	// attempted to access a not opened file }
        dsHandleInUse,		// attempted to reopen an already opened file }
        dsNoRoom,           // no room in the directory or data space }
        dsHandleInvalid,	// invalid file handle }
        dsNoData,           // attempted to read past the last record }
        dsIoError           // error in the Bios unit }
    };

    // file information }
    typedef struct { //    TFileInfo = record
          int dirindex; //: integer;		{ index of associated directory entry,
                         // value -1 when file not opened }
          int nextrec; //: cardinal;		{ next record number }
          int firstsec; //: cardinal;		{ starting sector }
          int lastrec; //: cardinal;		{ last accessed record... }
          int lastsec; //: cardinal;		{ ...and corresponding sector }
          int tag; //: integer;			{ no predefined meaning }
        } TFileInfo;

    CcasioDOS(CPObject *parent = 0);
    ~CcasioDOS();



    bool FormatDisk(void);
    bool CloseDiskFile(int handle);
    CcasioDOS::TDosStatusCode CheckFileHandle(int handle);

    TDosStatusCode DosStatus;
    TFileInfo fileinfo[MAX_FILES]; //: array [0..MAX_FILES-1] of TFileInfo;

};

#endif // CASIODISK_H
