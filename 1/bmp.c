#include <stdlib.h>
#include <memory.h>

#include "bmp.h"

#define BITS_IN_BYTE 8
#define HEADER_SIZE 14

void fread_dword_le(DWORD *dest, FILE  *f)
{
    BYTE c;
    *dest = 0;
    for (size_t i = 0; i < sizeof(DWORD); i++)
    {
        fread(&c, sizeof(BYTE), 1, f);
        (*dest) |= ((DWORD) c << (BITS_IN_BYTE * i));
    }
}

void fread_word_le(WORD *dest, FILE  *f)
{
    BYTE c;
    *dest = 0;
    for (size_t i = 0; i < sizeof(WORD); i++)
    {
        fread(&c, sizeof(BYTE), 1, f);
        (*dest) |= ((WORD) c << (BITS_IN_BYTE * i));
    }
}

void fwrite_dword_le(DWORD src, FILE  *f)
{
    BYTE c;
    for (size_t i = 0; i < sizeof(DWORD); i++)
    {
        c = (BYTE) (src >> (BITS_IN_BYTE * i));
        fwrite(&c, sizeof(BYTE), 1, f);
    }
}

void fwrite_word_le(WORD src, FILE  *f)
{
    BYTE c;
    for (size_t i = 0; i < sizeof(WORD); i++)
    {
        c = (BYTE) (src >> (BITS_IN_BYTE * i));
        fwrite(&c, sizeof(BYTE), 1, f);
    }
}

void bmp_copy_unchanged_header(PBMP src, PBMP dest)
{
    dest->header.biSize = src->header.biSize;
    dest->header.biPlanes = src->header.biPlanes;
    dest->header.biBitCount = src->header.biBitCount;
    dest->header.biCompression = src->header.biCompression;
    dest->header.biXPelsPerMeter = src->header.biXPelsPerMeter;
    dest->header.biYPelsPerMeter = src->header.biYPelsPerMeter;
    dest->header.biClrUsed = src->header.biClrUsed;
    dest->header.biClrImportant = src->header.biClrImportant;
}

void bmp_calculate_size(PBMP bmp)
{
    size_t padding_size = ((sizeof(LONG) - 3 * sizeof(BYTE) * 
                bmp->header.biWidth % sizeof(LONG)) % sizeof(LONG));
    bmp->header.biSizeImage = bmp->header.biHeight *
        (3 * bmp->header.biWidth * sizeof(BYTE) + padding_size);
}

void bmp_read(PBMP bmp, FILE *f)
{
    DWORD offset, tmp;

    // reading header
    fread(&tmp, sizeof(WORD), 1, f);  // bfType
    fread(&tmp, sizeof(DWORD), 1, f); // bfSize
    fread(&tmp, sizeof(WORD), 1, f);  // bfReserved1
    fread(&tmp, sizeof(WORD), 1, f);  // bfReserved2
    fread_dword_le(&offset, f);       // bfOffBits

    fread_dword_le(&bmp->header.biSize, f);
    fread_dword_le((DWORD*) &bmp->header.biWidth, f);
    fread_dword_le((DWORD*) &bmp->header.biHeight, f);
    fread_word_le(&bmp->header.biPlanes, f);
    fread_word_le(&bmp->header.biBitCount, f);
    fread_dword_le(&bmp->header.biCompression, f);
    fread_dword_le(&bmp->header.biSizeImage, f);
    fread_dword_le((DWORD*) &bmp->header.biXPelsPerMeter, f);
    fread_dword_le((DWORD*) &bmp->header.biYPelsPerMeter, f);
    fread_dword_le(&bmp->header.biClrUsed, f);
    fread_dword_le(&bmp->header.biClrImportant, f);

    // reading pixels
    size_t padding_size = ((sizeof(LONG) - 3 * sizeof(BYTE) * 
                bmp->header.biWidth % sizeof(LONG)) % sizeof(LONG));
    fseek(f, offset, SEEK_SET);
    bmp->pixels = malloc(abs(bmp->header.biHeight) * sizeof(BYTE*));
    for (LONG i = 0; i < abs(bmp->header.biHeight); i++)
    {
        LONG ri = (bmp->header.biHeight > 0 ? bmp->header.biHeight - 1 - i : i);
        bmp->pixels[ri] = malloc(3 * sizeof(BYTE) * bmp->header.biWidth);
        for (LONG j = 0; j < bmp->header.biWidth; j++)
            fread(bmp->pixels[ri] + j * 3, sizeof(BYTE), 3, f);
        fread(&tmp, padding_size, 1, f);
    }

    bmp->header.biHeight = abs(bmp->header.biHeight);
    bmp_calculate_size(bmp);
}

void bmp_write(PBMP bmp, FILE *f)
{
    DWORD zero = 0;
    size_t padding_size = ((sizeof(LONG) - 3 * sizeof(BYTE) * 
                bmp->header.biWidth % sizeof(LONG)) % sizeof(LONG));
    DWORD offset = HEADER_SIZE + bmp->header.biSize;

    // writing header
    fwrite("BM", sizeof(WORD), 1, f);                     // bfType
    fwrite_dword_le(offset + bmp->header.biSizeImage, f); // bfSize
    fwrite(&zero, sizeof(WORD), 1, f);                    // bfReserved1
    fwrite(&zero, sizeof(WORD), 1, f);                    // bfReserved2
    fwrite_dword_le(offset, f);                           // bfOffBits

    fwrite_dword_le(bmp->header.biSize, f);
    fwrite_dword_le((DWORD) bmp->header.biWidth, f);
    fwrite_dword_le((DWORD) bmp->header.biHeight, f);
    fwrite_word_le(bmp->header.biPlanes, f);
    fwrite_word_le(bmp->header.biBitCount, f);
    fwrite_dword_le(bmp->header.biCompression, f);
    fwrite_dword_le(bmp->header.biSizeImage, f);
    fwrite_dword_le((DWORD) bmp->header.biXPelsPerMeter, f);
    fwrite_dword_le((DWORD) bmp->header.biYPelsPerMeter, f);
    fwrite_dword_le(bmp->header.biClrUsed, f);
    fwrite_dword_le(bmp->header.biClrImportant, f);

    // writing pixels
    for (LONG i = 0; i < abs(bmp->header.biHeight); i++)
    {
        LONG ri = (bmp->header.biHeight > 0 ? bmp->header.biHeight - 1 - i : i);
        for (LONG j = 0; j < bmp->header.biWidth; j++)
            fwrite(bmp->pixels[ri] + j * 3, sizeof(BYTE), 3, f);
        fwrite(&zero, padding_size, 1, f);
    }
}

void bmp_crop(PBMP src, PBMP dest, int xa, int ya, int xb, int yb)
{
    dest->header.biWidth = xb - xa + 1;
    dest->header.biHeight = yb - ya + 1;

    dest->pixels = malloc(dest->header.biHeight * sizeof(BYTE*));
    for (LONG i = 0; i < dest->header.biHeight; i++)
    {
        dest->pixels[i] = malloc(3 * sizeof(BYTE) * dest->header.biWidth);
        memcpy(dest->pixels[i], src->pixels[ya + i] + 3 * xa, 3 * dest->header.biWidth);
    }

    bmp_copy_unchanged_header(src, dest);
    bmp_calculate_size(dest);
}

void bmp_rotate(PBMP src, PBMP dest)
{
    dest->header.biWidth = abs(src->header.biHeight);
    dest->header.biHeight = src->header.biWidth;

    dest->pixels = malloc(dest->header.biHeight * sizeof(BYTE*));
    for (LONG i = 0; i < dest->header.biHeight; i++)
    {
        dest->pixels[i] = malloc(3 * sizeof(BYTE) * dest->header.biWidth);
        for (LONG j = 0; j < dest->header.biWidth; j++)
            memcpy(dest->pixels[i] + 3 * j, 
                    src->pixels[dest->header.biWidth - 1 - j] + 3 * i, 
                    3 * sizeof(BYTE));
    }

    bmp_copy_unchanged_header(src, dest);
    bmp_calculate_size(dest);
}

void bmp_finit(PBMP bmp)
{
    for (LONG i = 0; i < abs(bmp->header.biHeight); i++)
        free(bmp->pixels[i]);
    free(bmp->pixels);
}
