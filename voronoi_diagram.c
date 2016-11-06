/*
 * Voronoi Diagram from http://rosettacode.org/wiki/Voronoi_diagram#C
 * with BMP output to stdout.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define N_SITES 150
double site[N_SITES][2];
unsigned char rgb[N_SITES][3];
 
int size_x = 640, size_y = 480;

double sq2(double x, double y)
{
    return x * x + y * y;
}

#define for_k for (k = 0; k < N_SITES; k++)
int nearest_site(double x, double y)
{
    int k, ret = 0;
    double d, dist = 0;
    for_k {
        d = sq2(x - site[k][0], y - site[k][1]);
        if (!k || d < dist) {
            dist = d, ret = k;
        }
    }
    return ret;
}

/* see if a pixel is different from any neighboring ones */
int at_edge(int *color, int y, int x)
{
    int i, j, c = color[y * size_x + x];
    for (i = y - 1; i <= y + 1; i++) {
        if (i < 0 || i >= size_y) continue;
 
        for (j = x - 1; j <= x + 1; j++) {
            if (j < 0 || j >= size_x) continue;
            if (color[i * size_x + j] != c) return 1;
        }
    }
    return 0;
}

#define AA_RES 4 /* average over 4x4 supersampling grid */
void aa_color(unsigned char *pix, int y, int x)
{
    int i, j, n;
    double r = 0, g = 0, b = 0, xx, yy;
    for (i = 0; i < AA_RES; i++) {
        yy = y + 1. / AA_RES * i + .5;
        for (j = 0; j < AA_RES; j++) {
            xx = x + 1. / AA_RES * j + .5;
            n = nearest_site(xx, yy);
            r += rgb[n][0];
            g += rgb[n][1];
            b += rgb[n][2];
        }
    }
    pix[0] = r / (AA_RES * AA_RES);
    pix[1] = g / (AA_RES * AA_RES);
    pix[2] = b / (AA_RES * AA_RES);
}
 
#define for_i for (i = 0; i < size_y; i++)
#define for_j for (j = 0; j < size_x; j++)
void gen_map()
{
    int i, j, k;
    int *nearest = malloc(sizeof(int) * size_y * size_x);
    unsigned char *ptr, *buf, color;
 
    ptr = buf = malloc(3 * size_x * size_y);
    for_i for_j nearest[i * size_x + j] = nearest_site(j, i);
 
    for_i for_j {
        if (!at_edge(nearest, i, j))
            memcpy(ptr, rgb[nearest[i * size_x + j]], 3);
        else    /* at edge, do anti-alias rastering */
            aa_color(ptr, i, j);
        ptr += 3;
    }
 
    /* draw sites */
    for (k = 0; k < N_SITES; k++) {
        color = (rgb[k][0]*.25 + rgb[k][1]*.6 + rgb[k][2]*.15 > 80) ? 0 : 255;
 
        for (i = site[k][1] - 1; i <= site[k][1] + 1; i++) {
            if (i < 0 || i >= size_y) continue;
 
            for (j = site[k][0] - 1; j <= site[k][0] + 1; j++) {
                if (j < 0 || j >= size_x) continue;
 
                ptr = buf + 3 * (i * size_x + j);
                ptr[0] = ptr[1] = ptr[2] = color;
            }
        }
    }
 
    // ppm format
    // printf("P6\n%d %d\n255\n", size_x, size_y);
    // fflush(stdout);
    // fwrite(buf, size_y * size_x * 3, 1, stdout);

    // bmp format
    unsigned char bmpfileheader[14] = {
        'B','M',    // magic
        0,0,0,0,    // size in bytes
        0,0,        // app data
        0,0,        // app data
        40+14,0,0,0 // start of data offset
    };
    unsigned char bmpinfoheader[40] = {
        40,0,0,0,   // info hd size
        0,0,0,0,    // width
        0,0,0,0,    // heigth
        1,0,        // number color planes
        24,0,       // bits per pixel
        0,0,0,0,    // compression is none
        0,0,0,0,    // image bits size
        0x13,0x0B,0,0, // horz resoluition in pixel / m
        0x13,0x0B,0,0, // vert resolutions (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
        0,0,0,0,    // #colors in pallete
        0,0,0,0,    // #important colors
    };
    unsigned char bmppad[3] = {0,0,0};

    int sizePad = (4-(size_x*3)%4)%4;
    int sizeData = sizeof(buf) + size_y*sizePad;
    int sizeAll = sizeData + sizeof(bmpfileheader) + sizeof(bmpinfoheader);

    bmpfileheader[ 2] = (unsigned char)(sizeAll);
    bmpfileheader[ 3] = (unsigned char)(sizeAll >>  8);
    bmpfileheader[ 4] = (unsigned char)(sizeAll >> 16);
    bmpfileheader[ 5] = (unsigned char)(sizeAll >> 24);

    bmpinfoheader[ 4] = (unsigned char)(size_x     );
    bmpinfoheader[ 5] = (unsigned char)(size_x >>  8);
    bmpinfoheader[ 6] = (unsigned char)(size_x >> 16);
    bmpinfoheader[ 7] = (unsigned char)(size_x >> 24);

    bmpinfoheader[ 8] = (unsigned char)(size_y     );
    bmpinfoheader[ 9] = (unsigned char)(size_y >>  8);
    bmpinfoheader[10] = (unsigned char)(size_y >> 16);
    bmpinfoheader[11] = (unsigned char)(size_y >> 24);

    bmpinfoheader[20] = (unsigned char)(sizeData     );
    bmpinfoheader[21] = (unsigned char)(sizeData >>  8);
    bmpinfoheader[22] = (unsigned char)(sizeData >> 16);
    bmpinfoheader[23] = (unsigned char)(sizeData >> 24);

    fflush(stdout);

    fwrite(bmpfileheader, 1, sizeof(bmpfileheader), stdout);
    fwrite(bmpinfoheader, 1, sizeof(bmpinfoheader), stdout);

    for (i=0; i<size_y; i++) {
        fwrite(buf+(size_x*(size_y-i-1)*3), 3, size_x, stdout);
        fwrite(bmppad, 1, sizePad, stdout);
    }

}
 
#define frand(x) (rand() / (1. + RAND_MAX) * x)
int main()
{
    int k;
    for_k {
        site[k][0] = frand(size_x);
        site[k][1] = frand(size_y);
        rgb [k][0] = frand(256);
        rgb [k][1] = frand(256);
        rgb [k][2] = frand(256);
    }
 
    gen_map();
    return 0;
}