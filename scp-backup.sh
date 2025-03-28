#!/bin/bash

# === CONFIGURATION ===
TARGET_USER="root"                        
TARGET_IP="192.168.0.14"             
TARGET_FILES=(
    "/etc/fstab"
    "/etc/inittab"
    "/opt/fibonacci"
)

LOCAL_BACKUP_DIR="./backup-files"

usage() {
    echo "Usage: $0 [backup|restore]"
    echo "  backup  --> Download target config files to host"
    echo "  restore --> Upload saved files back to target"
    exit 1
}

# === BACKUP FROM TARGET TO HOST ===
backup_files() {
    echo "Backing up target config files to host directory: $LOCAL_BACKUP_DIR"
    for file in "${TARGET_FILES[@]}"; do
        relative_path="${file#/}"  # Remove leading slash
        local_path="$LOCAL_BACKUP_DIR/$relative_path"
        local_dir=$(dirname "$local_path")

        echo "- Pulling $file from target..."
        mkdir -p "$local_dir"
        scp "$TARGET_USER@$TARGET_IP:$file" "$local_dir" 2>/dev/null || echo "/!\ Failed to get $file"
    done

    echo "Backup complete."
}

# === RESTORE FROM HOST TO TARGET ===
restore_files() {
    echo "Restoring config files from host to target..."
    for file in "${TARGET_FILES[@]}"; do
        relative_path="${file#/}"  # Remove leading slash
        local_path="$LOCAL_BACKUP_DIR/$relative_path"
        remote_dir=$(dirname "$file")

        if [ -f "$local_path" ]; then
            echo "- Creating remote dir: $remote_dir"
            ssh "$TARGET_USER@$TARGET_IP" "mkdir -p $remote_dir"
            echo "- Sending $relative_path to target:$file"
            scp "$local_path" "$TARGET_USER@$TARGET_IP:$remote_dir"
        else
            echo "/!\ File $filename not found in backup dir. Skipping."
        fi
    done
    echo "Restore complete."
}

# === MAIN ===
[ $# -ne 1 ] && usage

case "$1" in
    backup) backup_files ;;
    restore) restore_files ;;
    *) usage ;;
esac
