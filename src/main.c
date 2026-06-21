#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "scanner.h"
#include "backup.h"

int pipefd[2];
pid_t child_pid;

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\n[SYSTEM] Termination sequence captured. Flushing file tracking maps...\n");
        close(pipefd[0]);
        close(pipefd[1]);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory_to_monitor>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (pipe(pipefd) == -1) {
        perror("Failed to open internal IPC channel");
        return 1;
    }

    child_pid = fork();

    if (child_pid < 0) {
        perror("Multi-process instantiation failure");
        return 1;
    }

    if (child_pid == 0) {
        // Child Process Loop: Logger Subsystem
        close(pipefd[1]); 
        FileInfo input_buffer;

        printf("[CHILD/LOGGER] Asynchronous log listener online.\n");
        while (read(pipefd[0], &input_buffer, sizeof(FileInfo)) > 0) {
            printf("[LOG EVENT] Target File Detected: %s | Allocation Footprint: %ld bytes\n", 
                   input_buffer.name, input_buffer.size);
        }
        close(pipefd[0]);
        exit(0);
    } else {
        // Parent Process Loop: Scanner Subsystem
        close(pipefd[0]); 
        printf("[PARENT/SCANNER] Proactive surveillance loop initialized on target directory.\n");

        char source_file_path[512];
        
        // Emulated runtime tracking sweep
        scan_directory(argv[1], pipefd[1]);
        
        // Execute byte-level storage synchronization
        DIR *dir = opendir(argv[1]);
        struct dirent *entry;
        if (dir) {
            while ((entry = readdir(dir)) != NULL) {
                char *ext = strrchr(entry->d_name, '.');
                if (ext && strcmp(ext, ".txt") == 0) {
                    snprintf(source_file_path, sizeof(source_file_path), "%s/%s", argv[1], entry->d_name);
                    backup_file(source_file_path, entry->d_name);
                }
            }
            closedir(dir);
        }

        close(pipefd[1]);
        wait(NULL); 
    }

    return 0;
}
