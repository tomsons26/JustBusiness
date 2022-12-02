
class GraphicViewPortClass;

class BufferClass
{
public:
    BufferClass(void *ptr, long size);
    BufferClass(long size);
    BufferClass();
    ~BufferClass();

    long To_Page(GraphicViewPortClass &view);
    long To_Page(int w, int h, GraphicViewPortClass &view);
    long To_Page(int x, int y, int w, int h, GraphicViewPortClass &view);

    void *Get_Buffer();
    long Get_Size();

private:
    BufferClass(BufferClass const &);
    BufferClass &operator=(BufferClass const &);

protected:
    void *Buffer;
    long Size;
    int Allocated;
};

inline long BufferClass::Get_Size()
{
    return (Size);
}

inline void *BufferClass::Get_Buffer()
{
    return (Buffer);
}