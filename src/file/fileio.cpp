#include "_file.h"
#include "mapview.h"
#include <cstddef>

int ibm_open(char const *name, unsigned int mode, int attrib)
{
    int file_handle;

    int open_mode = 0;

    if (mode & 1) {
        open_mode |= 1;
    }

    if (mode & 2) {
        open_mode |= 2;
    }

    if (mode & 256) {
        ++CallingDOSInt;

        file_handle = _lcreat(name, 0);

        --CallingDOSInt;

        if (file_handle == -1) {
            file_handle = -1;
        }
        return file_handle;
    }

    ++CallingDOSInt;

    file_handle = _lopen(name, open_mode);

    --CallingDOSInt;

    if (file_handle == -1) {
        file_handle = -1;
    }

    return file_handle;
}
int ibm_close(int handle)
{
    ++CallingDOSInt;

    if (_lclose(handle) == -1) {
        --CallingDOSInt;
        return -1;
    }

    --CallingDOSInt;
    return 0;
}

unsigned int ibm_read(int handle, void *ptr, unsigned int bytes_to_read)
{
    ++CallingDOSInt;

    int bytes_read = _lread(handle, ptr, bytes_to_read);

    --CallingDOSInt;

    if (bytes_read == -1) {
        bytes_read = -1;
    }
    return bytes_read;
}

unsigned int ibm_write(int handle, void *ptr, unsigned int bytes_to_read)
{
    ++CallingDOSInt;

    int bytes_written = _lwrite(handle, (LPCCH)ptr, bytes_to_read);

    --CallingDOSInt;

    if (bytes_written == -1) {
        bytes_written = -1;
    }
    return bytes_written;
}

int ibm_unlink(char const *name)
{
    if (DeleteFile(name)) {
        return 0;
    }

    return -1;
}

int ibm_lseek(int handle, long offset, int where)
{
    ++CallingDOSInt;

    int seek_val = _llseek(handle, offset, where);

    --CallingDOSInt;

    if (seek_val == -1) {
        seek_val = -1;
    }
    return seek_val;
}
int ibm_chdir(char const *path)
{
    if (SetCurrentDirectory(path)) {
        return 0;
    }

    return -1;
}
// borland wrapper
extern int getdisk();

int ibm_getdisk()
{
    ++CallingDOSInt;

    int disk = getdisk();

    --CallingDOSInt;
    return disk;
}

int ibm_setdisk(int drive)
{
    int disk = 0;
    return disk;
}