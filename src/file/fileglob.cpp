#include "_file.h"

int Hard_Error_Occured = 0;
char MaxDevice;
char DefaultDrive;
FileDataType *FileDataPtr;
char MultiDriveSearch;
void *FileCacheHeap;

char ExecPath[XMAXPATH + 1];
char DataPath[XMAXPATH + 1];
char StartPath[XMAXPATH + 1];
int UseCD;
char CallingDOSInt;
FileHandleType FileHandleTable[TABLE_MAX];
unsigned int NumFiles;
unsigned int NumPAKFiles;
int DiskNumber;