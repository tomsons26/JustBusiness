#include "mcgaprim/gbuffer.h"
#define min(a, b) (((a) < (b)) ? (a) : (b))
#include "globals.h"

char *lpDib;

unsigned int GetDibInfoHeaderSize(char *lpDib)
{
    return ((BITMAPINFOHEADER *)lpDib)->biSize;
}

unsigned short GetDibWidth(char *lpDib)
{
    if (GetDibInfoHeaderSize(lpDib) == sizeof(BITMAPCOREHEADER)) {
        return (unsigned short)(((BITMAPCOREHEADER *)lpDib)->bcWidth);
    } else {
        return (unsigned short)(((BITMAPINFOHEADER *)lpDib)->biWidth);
    }
}

unsigned short GetDibHeight(char *lpDib)
{
    if (GetDibInfoHeaderSize(lpDib) == sizeof(BITMAPCOREHEADER)) {
        return (unsigned short)(((BITMAPCOREHEADER *)lpDib)->bcHeight);
    } else {
        return (unsigned short)(((BITMAPINFOHEADER *)lpDib)->biHeight);
    }
}

char *GetDibBitsAddr(char *lpDib)
{
    unsigned int dwNumColors;
    unsigned int dwColorTableSize;
    unsigned short wBitCount;

    if (GetDibInfoHeaderSize(lpDib) == sizeof(BITMAPCOREHEADER)) {
        wBitCount = ((BITMAPCOREHEADER *)lpDib)->bcBitCount;

        if (wBitCount != 24) {
            dwNumColors = 1 << wBitCount;
        } else {
            dwNumColors = 0;
        }

        dwColorTableSize = dwNumColors * sizeof(RGBTRIPLE);
    } else {
        wBitCount = ((BITMAPINFOHEADER *)lpDib)->biBitCount;

        if (GetDibInfoHeaderSize(lpDib) >= 36) {
            dwNumColors = ((BITMAPINFOHEADER *)lpDib)->biClrUsed;
        } else {
            dwNumColors = 0;
        }

        if (dwNumColors == 0) {
            if (wBitCount != 24) {
                dwNumColors = 1 << wBitCount;
            } else {
                dwNumColors = 0;
            }
        }

        dwColorTableSize = dwNumColors * sizeof(RGBQUAD);
    }

    return lpDib + GetDibInfoHeaderSize(lpDib) + dwColorTableSize;
}

char *ReadDib(const char *szFileName)
{
    BITMAPFILEHEADER bmfh;

    int hFile = _lopen(szFileName, OF_READ | OF_SHARE_DENY_WRITE);
    if (hFile == -1) {
        return NULL;
    }

    if (_lread(hFile, &bmfh, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER)) {
        _lclose(hFile);
        return NULL;
    }

    if (bmfh.bfType != *(WORD *)"BM") {
        _lclose(hFile);
        return NULL;
    }

    unsigned int dwDibSize = bmfh.bfSize - sizeof(BITMAPFILEHEADER);

    lpDib = (char *)GlobalLock(GlobalAlloc(GMEM_MOVEABLE, dwDibSize));

    if (lpDib == NULL) {
        _lclose(hFile);
        return NULL;
    }

    unsigned int dwOffset = 0;

    while (dwDibSize > 0) {
        unsigned short wDibRead = (unsigned short)min(32768u, dwDibSize);

        if (_lread(hFile, lpDib + dwOffset, wDibRead) != wDibRead) {
            _lclose(hFile);
            GlobalUnlock(GlobalHandle(lpDib));
            GlobalFree(GlobalHandle(lpDib));
            return NULL;
        }

        dwDibSize -= wDibRead;
        dwOffset += wDibRead;
    }

    _lclose(hFile);

    unsigned int dwHeaderSize = GetDibInfoHeaderSize(lpDib);

    if (dwHeaderSize < 12 || (dwHeaderSize > 12 && dwHeaderSize < 16)) {
        GlobalUnlock(GlobalHandle(lpDib));
        GlobalFree(GlobalHandle(lpDib));
        return NULL;
    }

    return lpDib;
}

void DIB_Load_Monopoly_BMP()
{
    if (lpDib) {
        GlobalUnlock(GlobalHandle(lpDib));
        GlobalFree(GlobalHandle(lpDib));
        lpDib = NULL;
    }

    lpDib = ReadDib("BOARD.BMP");

    if (lpDib == NULL) {
        MessageBoxA(Main_Window_Handle, "BOARD.BMP", "Could not open DIB file", MB_ICONEXCLAMATION | MB_OK);
    }
}
# if 0
void DIB_Copy_Monopoly_BMP_To_HID()
{
    if (lpDib) {
        char *lpDibBits = GetDibBitsAddr(lpDib);
        unsigned short cxDib = GetDibWidth(lpDib);
        unsigned short cyDib = GetDibHeight(lpDib);
        SetStretchBltMode(WinG_DC, COLORONCOLOR);
        StretchDIBits(
            WinG_DC, 0, 0, cxDib, cyDib, 0, 0, cxDib, cyDib, lpDibBits, (BITMAPINFO *)lpDib, DIB_RGB_COLORS, SRCCOPY);
    }
}

void DIB_Free_Monopoly_BMP()
{
    if (lpDib) {
        GlobalUnlock(GlobalHandle(lpDib));
        GlobalFree(GlobalHandle(lpDib));
    }
}
#endif