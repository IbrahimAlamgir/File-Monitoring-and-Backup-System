#ifndef SCANNER_H
#define SCANNER_H

typedef struct {
    char name[256];
    long size;
} FileInfo;

void scan_directory(const char *dir_path, int pipe_write_fd);

#endif
