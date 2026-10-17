#include <stdio.h>
#define BLOCK_SIZE 512
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s card.raw\n", argv[0]);
        return 1;
    }
    FILE *card = fopen(argv[1], "rb");
    if (card == NULL)
    {
        printf("Could not open card.raw\n");
        return 1;
    }
    unsigned char buffer[BLOCK_SIZE];
    FILE *out_file = NULL;
    int file_count = 0;
    char filename[8];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BLOCK_SIZE, card)) == BLOCK_SIZE)
    {
        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            if (out_file != NULL)
            {
                fclose(out_file);
            }
            sprintf(filename, "%03d.jpg", file_count);
            file_count++;

            out_file = fopen(filename, "wb");

            if (out_file == NULL)
            {
                printf("Could not create %s\n", filename);
                fclose(card);
                return 1;
            }
        }
        if (out_file != NULL)
        {
            if (fwrite(buffer, 1, BLOCK_SIZE, out_file) != BLOCK_SIZE)
            {
                printf("Error writing file\n");
                fclose(out_file);
                fclose(card);
                return 1;
            }
        }
    }
    if (ferror(card))
    {
        printf("Error reading card.raw\n");

        if (out_file != NULL)
        {
            fclose(out_file);
        }

        fclose(card);
        return 1;
    }
    if (out_file != NULL)
    {
        fclose(out_file);
    }

    fclose(card);

    printf("Recovered %d JPEG files\n", file_count);

    return 0;
}