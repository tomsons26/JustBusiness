// temp
#include "buffer.h"
#include "mapview.h"
#include <cstddef>

class GraphicViewPortClass;
class GraphicBufferClass;
class GraphicViewPortClass
{
public:
    GraphicViewPortClass(GraphicBufferClass *graphic_buff, int x, int y, int w, int h);
    GraphicViewPortClass();
    ~GraphicViewPortClass();

    int Get_Selector();

    long Get_Offset();

    int Get_Height();
    int Get_Width();

    int Get_XAdd();

    int Get_XPos();
    int Get_YPos();

    GraphicBufferClass *Get_Graphic_Buffer();

    int Change(int x, int y, int w, int h);

    void Put_Pixel(int x, int y, unsigned char color);
    int Get_Pixel(int x, int y);

    void Clear(unsigned char color = 0);

    long To_Buffer(int x, int y, int w, int h, void *buff, long size);
    long To_Buffer(int x, int y, int w, int h, BufferClass *buff);
    long To_Buffer(BufferClass *buff);

    HRESULT Blit(GraphicViewPortClass &dest, int x_pixel, int y_pixel, int dx_pixel, int dy_pixel, int pixel_width,
        int pixel_height, BOOL trans = FALSE);
    HRESULT Blit(GraphicViewPortClass &dest, int dx, int dy, BOOL trans = FALSE);
    HRESULT Blit(GraphicViewPortClass &dest, BOOL trans = FALSE);

    BOOL Scale(GraphicViewPortClass &dest, int src_x, int src_y, int dst_x, int dst_y, int src_w, int src_h, int dst_w,
        int dst_h, BOOL trans = FALSE, char *remap = NULL);
    BOOL Scale(GraphicViewPortClass &dest, int src_x, int src_y, int dst_x, int dst_y, int src_w, int src_h, int dst_w,
        int dst_h, char *remap);
    BOOL Scale(GraphicViewPortClass &dest, BOOL trans = FALSE, char *remap = NULL);
    BOOL Scale(GraphicViewPortClass &dest, char *remap);

    unsigned long Print(char const *string, int x_pixel, int y_pixel, int fcolor, int bcolor);
    unsigned long Print(short num, int x_pixel, int y_pixel, int fcol, int bcol);
    unsigned long Print(int num, int x_pixel, int y_pixel, int fcol, int bcol);
    unsigned long Print(long num, int x_pixel, int y_pixel, int fcol, int bcol);

    void Draw_Line(int sx, int sy, int dx, int dy, unsigned char color);

    void Draw_Rect(int sx, int sy, int dx, int dy, unsigned char color);

    void Fill_Rect(int sx, int sy, int dx, int dy, unsigned char color);

    void Fill_Quad(void *span_buff, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int color);

    void Remap(int sx, int sy, int width, int height, void *remap);
    void Remap(void *remap);

    void Attach(GraphicBufferClass *graphic_buff, int x, int y, int w, int h);
    void Attach(GraphicBufferClass *video_buff, int w, int h);

protected:
    int Selector;
    long Offset;
    int Width;
    int Height;
    int XAdd;
    int XPos;
    int YPos;
    GraphicBufferClass *GraphicBuff;
};

class GraphicBufferClass : public GraphicViewPortClass, public BufferClass
{
public:
    GraphicBufferClass(long size, int w, int h, void *buffer);
    GraphicBufferClass(int w, int h, void *buffer = 0);
    ~GraphicBufferClass();
};

inline int GraphicViewPortClass::Get_Selector()
{
    return Selector;
}

inline long GraphicViewPortClass::Get_Offset()
{
    return (Offset);
}

inline int GraphicViewPortClass::Get_Height()
{
    return (Height);
}

inline int GraphicViewPortClass::Get_Width()
{
    return (Width);
}

inline int GraphicViewPortClass::Get_XAdd()
{
    return (XAdd);
}

inline int GraphicViewPortClass::Get_XPos()
{
    return (XPos);
}

inline int GraphicViewPortClass::Get_YPos()
{
    return (YPos);
}

inline GraphicBufferClass *GraphicViewPortClass::Get_Graphic_Buffer()
{
    return (GraphicBuff);
}