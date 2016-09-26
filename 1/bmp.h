#ifndef __BMP_H__
#define __BMP_H__

#include <stdint.h>
#include <stdio.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagBMP
{
    BITMAPINFOHEADER header;
    BYTE **pixels;
} BMP, *PBMP;

void fread_dword_le(DWORD *dest, FILE *f); // also used for reading LONG
void fread_word_le(WORD  *dest, FILE *f);
void fwrite_dword_le(DWORD src, FILE *f); // also used for writing LONG

void fwrite_word_le(WORD src, FILE *f);

void bmp_read(PBMP bmp, FILE *f);
void bmp_write(PBMP bmp, FILE *f);

void bmp_crop(PBMP src, PBMP dest, int xa, int ya, int xb, int yb);
void bmp_rotate(PBMP src, PBMP dest);
void bmp_finit(PBMP bmp);
void bmp_calculate_size(PBMP bmp);
void bmp_copy_unchanged_header(PBMP src, PBMP dest);

#endif
