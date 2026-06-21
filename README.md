# Linux-Based Multi-Process File Monitoring and Backup System

An automated, system-level Linux utility engineered to monitor a target directory, process file information asynchronously, and manage secure file backups. The application transitions away from resource-intensive sequential scanning by leveraging low-level POSIX system calls, a multi-process architecture via `fork()`, and unidirectional Inter-Process Communication (IPC) pipes.


## System Architecture & Inter-Process Communication (IPC)

The core application splits tracking and logging overhead across two cooperative processes to maximize processing efficiency and maintain isolation boundaries:

* **Parent Process (Directory Scanner):** Utilizes `dirent.h` API vectors to continually traverse target workspace directory structures. It filters explicitly for `.txt` extensions, extracts file metadata, and handles backup initiation via custom file I/O routines.
* **Child Process (Information Logger):** Dedicated entirely to execution auditing. It operates asynchronously, receiving file information streams from the parent process over a dedicated IPC channel and flushing formatted logs to disk.
* **IPC Lane (Unidirectional Pipe):** A single POSIX pipe coordinates the processes. The parent binds to the write-end (`pipefd[1]`) to transmit structured metadata blocks, while the child blocks on the read-end (`pipefd[0]`) waiting for payload ingest.

### Process Topology & Data Stream

  [ Parent Process: Scanner ] 
         |
         |---> (Metadata Stream) ---> [ POSIX Pipe (pipefd) ] ---> [ Child Process: Logger ]
         |                                                                  |
         v                                                                  v
  [ /src/ (Read Target) ] ---> [ /backup/ (Byte Copy) ]            [ /logs/report.txt ]

## Software Component Matrix & Design

| Module / File | Operational Responsibility | Core System Level Interfaces |
| :--- | :--- | :--- |
| `src/main.c` | Orchestrates system lifecycles, instantiates IPC pipelines, forks worker nodes, and binds POSIX signal handlers. | `pipe()`, `fork()`, `signal()`, `wait()` |
| `src/scanner.c` | Handles structural traversal of targeted path systems and extracts file characteristics dynamically. | `opendir()`, `readdir()`, `stat()` |
| `src/backup.c` | Executes low-level binary stream copies to mirror file modifications safely into storage locations. | `fopen()`, `fgetc()`, `fputc()` |
| `scripts/run_system.sh` | Bash automation interface driving environmental configurations, validation checks, and compilation routines. | Bash Shell Syntax, Exit Codes (`$?`) |
| `Makefile` | Multi-module build system script managing macro-level translation, dependencies, and clean-up directives. | `gcc -Wall -Wextra` |

### Architectural Robustness Features
* **Memory Management:** Heap configurations allocate and free references dynamically using `FileInfo` abstractions to avoid pointer corruption during dense directory sweeps.
* **Graceful Termination:** Binds system-level signal listeners (`SIGINT` / `SIGTERM`) to clean up open file descriptors, release allocated tracking descriptors, and flush standard output channels before closing out.
* **Error Prevention:** Performs null-pointer validation checks across all file allocation descriptors (`fopen`) to eliminate segmentation faults if storage sectors fail or drop offline.

### System Validation & Operational Verification

* **Test Case 1 (Directory Ingest & Filtering):** Placing multiple extension types (`.log`, `.txt`, `.bin`) into the monitor path confirms that only targeted text payloads trigger parsing. **[Pass]**
* **Test Case 2 (Asynchronous Pipe Communication):** Simulating real-time updates verifies that metadata structures cross the POSIX pipe smoothly without dropping blocks or blocking the parent loop. **[Pass]**
* **Test Case 3 (Byte-Level Storage Mirroring):** Running structural comparisons on original and backup files confirms bit-for-bit data parity across text storage channels. **[Pass]**
* **Test Case 4 (Automated Build & Execution Pipeline):**
    * *Issue:* Early build environments threw runtime execution errors when automated testing components launched prior to the physical directory structures being provisioned.
    * *Resolution:* Upgraded `run_system.sh` to introduce proactive structural checks (`mkdir -p`) and capture compilation error flags explicitly before executing any binary logic. **[Pass]**

### Project Context & References

Developed for **BCS 323 – System Level Programming** under the academic guidance of Dr. Mehak Khurana, Mr. Adel, and Mr. Hussein.

* Kerridge, M., & Love, J. (2020). *Linux System Programming: Low-Level Core Interfaces*.
* IEEE Standard 1003.1. (2018). *POSIX System Interfaces Specification (Pipes, Forking, Directory Stepping)*.
