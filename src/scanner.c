#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "scanner.h"

void scan_directory(const char *dir_path, int pipe_write_fd) {
    DIR *dir = opendir(dir_path);
    struct dirent *entry;
    struct stat file_stat;
    char full_path[512];

    if (!dir) {
        perror("Failed to access targeted directory path");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char *extension = strrchr(entry->d_name, '.');
            if (extension && strcmp(extension, ".txt") == 0) {
                snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
                if (stat(full_path, &file_stat) == 0) {
                    FileInfo info;
                    strncpy(info.name, entry->d_name, sizeof(info.name));
                    info.size = file_stat.st_size;

                    write(pipe_write_fd, &info, sizeof(FileInfo));
                }
            }
        }
    }
    closedir(dir);
}
