#include "globals.h"
#include "mapview.h"
#include <cstddef>

#include <cstdlib>
extern void *WinG_Buffer_Ptr;

#if 0
void Put_Pixel(int x_pix, int y_pix, unsigned char color)
{
    unsigned char *buffer;

    if (DIB_Top_Down) {
        buffer = (unsigned char *)WinG_Buffer_Ptr + 640 * y_pix;
    } else {
        buffer = (unsigned char *)WinG_Buffer_Ptr - 640 * y_pix + 280960;
    }
    buffer[x_pix] = color;
}

unsigned char Get_Pixel(int x_pix, int y_pix)
{
    unsigned char *buffer;

    if (DIB_Top_Down) {
        buffer = (unsigned char *)WinG_Buffer_Ptr + 640 * y_pix;
    } else {
        buffer = (unsigned char *)WinG_Buffer_Ptr - 640 * y_pix + 280960;
    }
    return buffer[x_pix];
}
#if 0
const int *max(const int *a1, const int *a2)
{
    const int *result = a2;
    if (*a1 > *a2) {
        result = a1;
    }
    return result;
}
#endif

void Horizontal_Line(int x1, int y_pix, int x2, unsigned char color)
{
    int min_x;
    unsigned char *buffer;

    if (x1 >= x2) {
        min_x = x2;
    } else {
        min_x = x1;
    }

    int max_x = *max(&x1, &x2);
    x1 = min_x;
    x2 = max_x;

    if (DIB_Top_Down) {
        buffer = (unsigned char *)WinG_Buffer_Ptr + 640 * y_pix;
    } else {
        buffer = (unsigned char *)WinG_Buffer_Ptr - 640 * y_pix + 280960;
    }

    for (int i = x1; i <= x2; ++i) {
        buffer[x1] = color;
        buffer++;
    }
}

void Vertical_Line(int x_pix, int y1, int y2, unsigned char color)
{
    int min_y;
    unsigned char *buffer;
    int adder;

    if (y1 < y2) {
        min_y = y1;
    } else {
        min_y = y2;
    }

    int max_y = *max(&y1, &y2);
    y1 = min_y;
    y2 = max_y;

    if (DIB_Top_Down) {
        buffer = (unsigned char *)WinG_Buffer_Ptr + 640 * y1 + x_pix;
        adder = 640;
    } else {
        buffer = (unsigned char *)WinG_Buffer_Ptr + x_pix + -640 * y1 + 280960;
        adder = -640;
    }

    for (int i = y1; i <= y2; ++i) {
        *buffer = color;
        buffer += adder;
    }
}

void Draw_Box(int x1, int y1, int x2, int y2, unsigned char color, int thickness)
{
    int min_x;
    int min_y;

    if (x1 < x2) {
        min_x = x1;
    } else {
        min_x = x2;
    }

    int max_x = *max(&x1, &x2);
    x1 = min_x;
    x2 = max_x;

    if (y1 < y2) {
        min_y = y1;
    } else {
        min_y = y2;
    }

    int max_y = *max(&y1, &y2);
    y1 = min_y;
    y2 = max_y;

    for (int i = 0; thickness > i; ++i) {
        Horizontal_Line(x1, y1, x2, color);
        Vertical_Line(x1, y1, y2, color);
        Vertical_Line(x2, y1, y2, color);
        Horizontal_Line(x1++, y2, x2, color);
        ++y1;
        --x2;
        --y2;
    }
}

void Fill_Rect(int x1, int y1, int x2, int y2, unsigned char color)
{
    int min_x;
    int min_y;
    unsigned char *buffer;
    int adder;

    if (x1 < x2) {
        min_x = x1;
    } else {
        min_x = x2;
    }

    int max_x = *max(&x1, &x2);
    x1 = min_x;
    x2 = max_x;
    if (y1 < y2) {
        min_y = y1;
    } else {
        min_y = y2;
    }

    int max_y = *max(&y1, &y2);
    y1 = min_y;
    y2 = max_y;
    int width = x2 - x1 + 1;

    if (DIB_Top_Down) {
        buffer = (unsigned char *)WinG_Buffer_Ptr + 640 * y1 + x1;
        adder = 640 - width;
    } else {
        buffer = (unsigned char *)WinG_Buffer_Ptr + x1 + -640 * y1 + 280960;
        adder = -(x2 - x1 + 641);
    }

    for (int i = y1; i <= y2; ++i) {
        for (int j = 0; width > j; ++j) {
            *buffer++ = color;
        }
        buffer += adder;
    }
}

void Draw_Line(int x1, int y1, int x2, int y2, unsigned char color)
{
    int sdy;
    int sdx;

    int py = y1;
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dx > 0) {
        sdx = 1;
    } else {
        sdx = 0;
        if (x2 != x1) {
            sdx = -1;
        }
    }

    if (dy > 0) {
        sdy = 1;
    } else {
        sdy = 0;
        if (y2 != y1) {
            sdy = -1;
        }
    }

    int dxabs = abs(dx);
    int dyabs = abs(dy);
    int x = 0;
    int y = 0;
    int px = x1;

    if (dyabs <= dxabs) {
        for (int i = 0; i <= dxabs; ++i) {
            y += dyabs;
            if (dxabs < y) {
                y -= dxabs;
                py += sdy;
            }
            Put_Pixel(px, py, color);
            px += sdx;
        }
    } else {
        for (int i = 0; i <= dyabs; ++i) {
            x += dxabs;
            if (dyabs < x) {
                x -= dyabs;
                px += sdx;
            }
            Put_Pixel(px, py, color);
            py += sdy;
        }
    }
}

void FUNC_00428F1E(LONG left, LONG top, LONG right, LONG bottom)
{
    RECT rc;
    HBRUSH brush;

    HDC dc = GetDC(Main_Window_Handle);
    rc.left = left;
    rc.top = top;
    rc.right = right;
    rc.bottom = bottom;

    if (0 /*HidBuff_Class_Ptr && !Running_Demo_Attract_Mode && !Waiting_For_Initial_Game_Click*/) {
        brush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    } else {
        brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    }
    FillRect(dc, &rc, brush);

    ReleaseDC(Main_Window_Handle, dc);
}

int set_font = 0;
HFONT testfont1;
HFONT testfont2;

void __stdcall FUNC_00428FE2(int arg1)
{
    LOGFONT font2;
    LOGFONT font1;
    tagSIZE size;

    if (!set_font) {
        set_font = 1;

        font1.lfEscapement = 0;
        font1.lfOrientation = 0;
        font1.lfQuality = DEFAULT_QUALITY;
        font1.lfOutPrecision = OUT_STRING_PRECIS;
        font1.lfClipPrecision = CLIP_STROKE_PRECIS;
        font1.lfItalic = FALSE;
        font1.lfUnderline = FALSE;
        font1.lfStrikeOut = FALSE;
        font1.lfHeight = 16;
        font1.lfWidth = 7;
        font1.lfWeight = 700;
        font1.lfCharSet = ANSI_CHARSET;
        font1.lfPitchAndFamily = (VARIABLE_PITCH | FF_SWISS);

        font2.lfEscapement = 0;
        font2.lfOrientation = 0;
        font2.lfQuality = DEFAULT_QUALITY;
        font2.lfOutPrecision = OUT_STRING_PRECIS;
        font2.lfClipPrecision = CLIP_STROKE_PRECIS;
        font2.lfItalic = FALSE;
        font2.lfUnderline = FALSE;
        font2.lfStrikeOut = FALSE;
        font2.lfHeight = 16;
        font2.lfWidth = 7;
        font2.lfWeight = 400;
        font2.lfCharSet = ANSI_CHARSET;
        font2.lfPitchAndFamily = (VARIABLE_PITCH | FF_SWISS);

        testfont1 = CreateFontIndirectA(&font1);
        testfont2 = CreateFontIndirectA(&font2);
        strcpy(Text_String, "This is a test of string widths.");
        SelectObject(WinG_DC, GetStockObject(arg1));
        GetTextExtentPointA(WinG_DC, Text_String, strlen(Text_String), &size);
        int x = size.cx;

        SelectObject(WinG_DC, testfont1);
        GetTextExtentPointA(WinG_DC, Text_String, strlen(Text_String), &size);
        if (size.cx >= x) {
            DeleteObject(testfont1);
            DeleteObject(testfont2);
            testfont1 = NULL;
            testfont2 = NULL;
        }
    }

    if (arg1 == 13) {
        if (testfont1) {
            SelectObject(WinG_DC, testfont1);
        } else {
            SelectObject(WinG_DC, GetStockObject(arg1));
        }
    } else if (arg1 == 12) {
        if (testfont2) {
            SelectObject(WinG_DC, testfont2);
        } else {
            SelectObject(WinG_DC, GetStockObject(arg1));
        }
    }
}

void FUNC_004291B1(int x1, int y1, int x2, int y2, unsigned char color)
{
    for (int y = y1; y <= y2; ++y) {
        if (y & 1) {
            for (int x = x1 + 1; x2 >= x; x += 2) {
                Put_Pixel(x, y, color);
            }
        } else {
            for (int x = x1; x2 >= x; x += 2) {
                Put_Pixel(x, y, color);
            }
        }
    }
}
#endif