
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *file2strl(const char *path, unsigned int *file_len_out)
{
    FILE *file;
    int e;

    file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "Unable to open file %s\n", path);
        return NULL;
    }

    e = fseek(file, 0, SEEK_END);
    if (-1 == e)
    {
        fprintf(stderr, "Unable to seek file %s\n", path);
        fclose(file);
        return NULL;
    }

    long file_len = ftell(file);
    if (-1 == file_len)
    {
        fprintf(stderr, "Unable to ftell() file %s\n", path);
        fclose(file);
        return NULL;
    }

    e = fseek(file, 0, SEEK_SET);
    if (-1 == e)
    {
        fprintf(stderr, "Unable to seek file %s\n", path);
        fclose(file);
        return NULL;
    }

    char *contents = malloc(file_len + 1);
    if (!contents)
    {
        fprintf(stderr, "Memory error!\n");
        fclose(file);
        return NULL;
    }

    unsigned long bytes_read = fread(contents, file_len, 1, file);
    if (bytes_read == 0 && ferror(file))
    {
        fprintf(stderr, "Read error");
        free(contents);
        fclose(file);
        return NULL;
    }
    fclose(file);

    contents[file_len] = '\0';

    if (file_len_out)
        *file_len_out = file_len + 1;

    return contents;
}

char *file2str(const char *path)
{
    return file2strl(path, NULL);
}
