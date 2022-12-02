unsigned int __cdecl Row_Based_RLE_Decompress_Top_Down(
    unsigned char *source, unsigned char *dest, unsigned int source_size, int surface_width, int row_width)
{
    unsigned int src_count = 0;
    unsigned int dest_count = 0;
    unsigned char *src_ptr = source;
    unsigned char *dest_ptr = dest;

    while (src_count < source_size) {
        unsigned char src_byte = *src_ptr++;
        ++src_count;
        if (src_byte & 128) {
            unsigned int dump_length = src_byte & 127;
            for (unsigned int i = 0; dump_length > i; ++i) {
                *dest_ptr++ = *src_ptr++;
            }
            src_count += dump_length;
            dest_count += dump_length;
        } else if (!src_byte) {
            dest_ptr += surface_width - row_width;
        } else {
            unsigned int run_length = src_byte;
            src_byte = *src_ptr++;
            ++src_count;
            for (unsigned int i = 0; run_length > i; ++i) {
                *dest_ptr++ = src_byte;
            }
            dest_count += run_length;
        }
    }

    return dest_count;
}

unsigned int __cdecl Row_Based_RLE_Decompress_Bottom_Up(
    unsigned char *source, unsigned char *dest, unsigned int source_size, int surface_width, int row_width)
{
    unsigned int src_count = 0;
    unsigned int dest_count = 0;
    unsigned char *src_ptr = source;
    unsigned char *dest_ptr = dest;

    while (src_count < source_size) {
        unsigned char src_byte = *src_ptr++;
        ++src_count;
        if (src_byte & 128) {
            unsigned int dump_length = src_byte & 127;
            for (unsigned int i = 0; dump_length > i; ++i) {
                *dest_ptr++ = *src_ptr++;
            }
            src_count += dump_length;
            dest_count += dump_length;
        } else if (!src_byte) {
            dest_ptr += -row_width - surface_width;
        } else {
            unsigned int run_length = src_byte;
            src_byte = *src_ptr++;
            ++src_count;
            for (unsigned int i = 0; run_length > i; ++i) {
                *dest_ptr++ = src_byte;
            }
            dest_count += run_length;
        }
    }

    return dest_count;
}