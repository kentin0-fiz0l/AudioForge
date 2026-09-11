#!/bin/bash
# Clean up old plugin backups

VST3_DIR="$HOME/Library/Audio/Plug-Ins"

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  AudioForge Backup Cleanup"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Find backup directories
BACKUP_DIRS=$(find "$VST3_DIR" -maxdepth 1 -name "VST3_Backup_*" -type d 2>/dev/null | sort)
BACKUP_FILES=$(find "$VST3_DIR/VST3" -maxdepth 1 -name "*.backup.*" -type d 2>/dev/null | sort)

if [ -z "$BACKUP_DIRS" ] && [ -z "$BACKUP_FILES" ]; then
    echo "  ✓ No backups found - nothing to clean up!"
    echo ""
    exit 0
fi

echo "Found backups:"
echo ""

TOTAL_SIZE=0
COUNT=0

# List backup directories
if [ ! -z "$BACKUP_DIRS" ]; then
    echo "Backup directories:"
    while IFS= read -r backup; do
        if [ -d "$backup" ]; then
            size=$(du -sh "$backup" | cut -f1)
            size_kb=$(du -sk "$backup" | cut -f1)
            TOTAL_SIZE=$((TOTAL_SIZE + size_kb))
            COUNT=$((COUNT + 1))
            echo "  - $(basename "$backup") ($size)"
        fi
    done <<< "$BACKUP_DIRS"
    echo ""
fi

# List backup files
if [ ! -z "$BACKUP_FILES" ]; then
    echo "Individual backups:"
    while IFS= read -r backup; do
        if [ -d "$backup" ]; then
            size=$(du -sh "$backup" | cut -f1)
            size_kb=$(du -sk "$backup" | cut -f1)
            TOTAL_SIZE=$((TOTAL_SIZE + size_kb))
            COUNT=$((COUNT + 1))
            echo "  - $(basename "$backup") ($size)"
        fi
    done <<< "$BACKUP_FILES"
    echo ""
fi

TOTAL_SIZE_MB=$((TOTAL_SIZE / 1024))
echo "Total: $COUNT backups, ${TOTAL_SIZE_MB} MB"
echo ""

# Ask for confirmation
read -p "Delete all backups? [y/N] " -n 1 -r
echo ""

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    echo "Deleting backups..."

    # Delete backup directories
    if [ ! -z "$BACKUP_DIRS" ]; then
        while IFS= read -r backup; do
            if [ -d "$backup" ]; then
                echo "  Deleting: $(basename "$backup")"
                rm -rf "$backup"
            fi
        done <<< "$BACKUP_DIRS"
    fi

    # Delete backup files
    if [ ! -z "$BACKUP_FILES" ]; then
        while IFS= read -r backup; do
            if [ -d "$backup" ]; then
                echo "  Deleting: $(basename "$backup")"
                rm -rf "$backup"
            fi
        done <<< "$BACKUP_FILES"
    fi

    echo ""
    echo "✅ Cleaned up ${TOTAL_SIZE_MB} MB of backups"
else
    echo "Cancelled - backups preserved"
fi

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
