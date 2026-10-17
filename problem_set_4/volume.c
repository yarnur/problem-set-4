#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define HEADER_SIZE 44
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: ./volume input.wav output.wav factor\n");
        return 1;
    }

    FILE *input = fopen(argv[1], "rb");
    if (input == NULL)
    {
        printf("Could not open input file.\n");
        return 1;
    }

    FILE *output = fopen(argv[2], "wb");
    if (output == NULL)
    {
        printf("Could not open output file.\n");
        fclose(input);
        return 1;
    }

    float factor = atof(argv[3]);

    unsigned char header[HEADER_SIZE];

    fread(header, sizeof(unsigned char), HEADER_SIZE, input);
    fwrite(header, sizeof(unsigned char), HEADER_SIZE, output);

    int16_t buffer;

    while (fread(&buffer, sizeof(int16_t), 1, input) == 1)
    {
        float sample = buffer * factor;

        if (sample > 32767)
        {
            sample = 32767;
        }
        if (sample < -32768)
        {
            sample = -32768;
        }

        buffer = sample;

        fwrite(&buffer, sizeof(int16_t), 1, output);
    }

    fclose(input);
    fclose(output);

    return 0;
}
