#include <stdio.h>
#pragma pack(push, 1)
typedef struct
{
    unsigned short type;
    unsigned int size;
    unsigned short r1, r2;
    unsigned int offset;
} FILEHEADER;
typedef struct
{
    unsigned int size;
    int width, height;
    unsigned short planes, bits;
    unsigned int compression, imageSize;
    int x, y;
    unsigned int colors, important;
} INFOHEADER;
typedef struct
{
    unsigned char blue, green, red;
} RGBTRIPLE;
#pragma pack(pop)
void grayscale(int h, int w, RGBTRIPLE img[h][w])
{
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
        {
            int avg = (img[i][j].red + img[i][j].green +
                       img[i][j].blue) / 3;

            img[i][j].red = avg;
            img[i][j].green = avg;
            img[i][j].blue = avg;
        }
}
void sepia(int h, int w, RGBTRIPLE img[h][w])
{
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
        {
            int r = img[i][j].red;
            int g = img[i][j].green;
            int b = img[i][j].blue;

            int red = 0.393 * r + 0.769 * g + 0.189 * b;
            int green = 0.349 * r + 0.686 * g + 0.168 * b;
            int blue = 0.272 * r + 0.534 * g + 0.131 * b;

            img[i][j].red = red > 255 ? 255 : red;
            img[i][j].green = green > 255 ? 255 : green;
            img[i][j].blue = blue > 255 ? 255 : blue;
        }
}
void reflect(int h, int w, RGBTRIPLE img[h][w])
{
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w / 2; j++)
        {
            RGBTRIPLE temp = img[i][j];
            img[i][j] = img[i][w - 1 - j];
            img[i][w - 1 - j] = temp;
        }
}
void blur(int h, int w, RGBTRIPLE img[h][w])
{
    RGBTRIPLE copy[h][w];
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            copy[i][j] = img[i][j];
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
        {
            int r = 0, g = 0, b = 0, count = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                {
                    int y = i + di;
                    int x = j + dj;
                    if (y >= 0 && y < h && x >= 0 && x < w)
                    {
                        r += copy[y][x].red;
                        g += copy[y][x].green;
                        b += copy[y][x].blue;
                        count++;
                    }
                }
            img[i][j].red = (r + count / 2) / count;
            img[i][j].green = (g + count / 2) / count;
            img[i][j].blue = (b + count / 2) / count;
        }
}
int main(int argc, char *argv[])
{
    if (argc != 4)
        return 1;
    FILE *in = fopen(argv[2], "rb");
    FILE *out = fopen(argv[3], "wb");
    if (in == NULL || out == NULL)
    {
        if (in != NULL) fclose(in);
        if (out != NULL) fclose(out);
        return 1;
    }
    FILEHEADER fh;
    INFOHEADER ih;
    if (fread(&fh, sizeof(fh), 1, in) != 1 ||
        fread(&ih, sizeof(ih), 1, in) != 1)
    {
        fclose(in);
        fclose(out);
        return 1;
    }
    int w = ih.width;
    int h = ih.height < 0 ? -ih.height : ih.height;
    if (w <= 0 || h <= 0 || ih.bits != 24 ||
        ih.compression != 0 || fh.offset != 54 ||
        ih.height == (-2147483647 - 1))
    {
        fclose(in);
        fclose(out);
        return 1;
    }
    int padding = (4 - (w * 3) % 4) % 4;
    RGBTRIPLE img[h][w];
    for (int i = 0; i < h; i++)
    {
        if (fread(img[i], sizeof(RGBTRIPLE), w, in) != (size_t) w)
        {
            fclose(in);
            fclose(out);
            return 1;
        }

        fseek(in, padding, SEEK_CUR);
    }
    switch (argv[1][0])
    {
        case 'g': grayscale(h, w, img); break;
        case 's': sepia(h, w, img); break;
        case 'r': reflect(h, w, img); break;
        case 'b': blur(h, w, img); break;
        default:
            fclose(in);
            fclose(out);
            return 1;
    }
    fwrite(&fh, sizeof(fh), 1, out);
    fwrite(&ih, sizeof(ih), 1, out);
    for (int i = 0; i < h; i++)
    {
        fwrite(img[i], sizeof(RGBTRIPLE), w, out);

        for (int j = 0; j < padding; j++)
            fputc(0, out);
    }
    fclose(in);
    fclose(out);
    return 0;
}