#include <stdio.h>
#include <stdlib.h>
#include "backup.h"

void backup_file(const char *source_path, const char *filename) {
    char destination_path[512];
    snprintf(destination_path, sizeof(destination_path), "backup/%s", filename);

    FILE *src = fopen(source_path, "rb");
    FILE *dst = fopen(destination_path, "wb");

    if (!src || !dst) {
        if (src) fclose(src);
        if (dst) fclose(dst);
        return;
    }

    int ch;
    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dst);
    }

    fclose(src);
    fclose(dst);
}
