# Linux-Based Multi-Process File Monitoring and Backup System

An automated, system-level Linux utility engineered to monitor a target directory, process file information asynchronously, and manage secure file backups. The application transitions away from resource-intensive sequential scanning by leveraging low-level POSIX system calls, a multi-process architecture via `fork()`, and unidirectional Inter-Process Communication (IPC) pipes.


## System Architecture & Inter-Process Communication (IPC)

The core application splits tracking and logging overhead across two cooperative processes to maximize processing efficiency and maintain isolation boundaries:

* **Parent Process (Directory Scanner):** Utilizes `dirent.h` API vectors to continually traverse target workspace directory structures. It filters explicitly for `.txt` extensions, extracts file metadata, and handles backup initiation via custom file I/O routines.
* **Child Process (Information Logger):** Dedicated entirely to execution auditing. It operates asynchronously, receiving file information streams from the parent process over a dedicated IPC channel and flushing formatted logs to disk.
* **IPC Lane (Unidirectional Pipe):** A single POSIX pipe coordinates the processes. The parent binds to the write-end (`pipefd[1]`) to transmit structured metadata blocks, while the child blocks on the read-end (`pipefd[0]`) waiting for payload ingest.

### Process Topology & Data Stream
```text
  [ Parent Process: Scanner ] 
         |
         |---> (Metadata Stream) ---> [ POSIX Pipe (pipefd) ] ---> [ Child Process: Logger ]
         |                                                                  |
         v                                                                  v
  [ /src/ (Read Target) ] ---> [ /backup/ (Byte Copy) ]            [ /logs/report.txt ]
