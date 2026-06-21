#!/bin/bash

TARGET_DIR="monitored_workspace"
BACKUP_DIR="backup"
LOG_DIR="logs"
REPORT="$LOG_DIR/report.txt"

echo "[SYSTEM] Initializing operating environment..."
mkdir -p "$TARGET_DIR" "$BACKUP_DIR" "$LOG_DIR"

echo "Test system payload file creation." > "$TARGET_DIR/sample1.txt"
echo "Secondary multi-process validation string." > "$TARGET_DIR/sample2.txt"

echo "[SYSTEM] Initiating compilation via build system..."
make clean && make
if [ $? -ne 0 ]; then
    echo "[ERROR] Code compilation encountered structural failure."
    exit 1
fi

echo "[SYSTEM] Executing tracking processes. Routing data output to $REPORT..."
./monitor_system "$TARGET_DIR" > "$REPORT" 2>&1 &
SYSTEM_PID=$!

sleep 3

echo "[SYSTEM] Execution loop verified. Sending shutdown signal to process ID: $SYSTEM_PID"
kill -2 $SYSTEM_PID

echo "[SYSTEM] Environment audit completed successfully."
cat "$REPORT"
