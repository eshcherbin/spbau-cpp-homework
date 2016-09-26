#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bmp.h"

int xa, ya, xb, yb;
BMP bmp_in, bmp_tmp, bmp_out;

void print_error_msg()
{
    fprintf(stderr, "Wrong format of arguments\n"
            "Usage: ./main in_file.bmp out_file.bmp "
            "x0 y0 x1 y1\nin_file.bmp  -- source file\n"
            "out_file.bmp -- destination file\n"
            "x0 y0 x1 y1  -- coordinates of a rectangle inside in_file.bmp\n");
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 7)
        print_error_msg();

    FILE *in = fopen(argv[1], "r");
    if (!in)
    {
        fprintf(stderr, "Couldn't open file: %s\n", argv[1]);
        exit(2);
    }
    bmp_read(&bmp_in, in);
    fclose(in);

    char tmp;
    if (sscanf(argv[3], "%d%c", &xa, &tmp) != 1 ||
            sscanf(argv[4], "%d%c", &ya, &tmp) != 1 ||
            sscanf(argv[5], "%d%c", &xb, &tmp) != 1 ||
            sscanf(argv[6], "%d%c", &yb, &tmp) != 1 ||
            xa > xb || ya > yb ||
            !(0 <= xa && xb < bmp_in.header.biWidth) ||
            !(0 <= ya && yb < abs(bmp_in.header.biHeight)))
        print_error_msg();

    bmp_crop(&bmp_in, &bmp_tmp, xa, ya, xb, yb);
    bmp_rotate(&bmp_tmp, &bmp_out);

    FILE *out = fopen(argv[2], "w");
    bmp_write(&bmp_out, out);
    fclose(out);

    bmp_finit(&bmp_in);
    bmp_finit(&bmp_out);
    bmp_finit(&bmp_tmp);

    return 0;
}
