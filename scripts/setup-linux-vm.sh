#!/bin/bash
# AudioForge Linux VM Setup Script
# Automates Multipass VM creation and package building

set -e

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}AudioForge Linux VM Setup${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if Multipass is installed
if ! command -v multipass &> /dev/null; then
    echo -e "${YELLOW}Multipass not found. Installing...${NC}"
    echo "Please run: brew install multipass"
    echo "Then run this script again."
    exit 1
fi

VM_NAME="audioforge-build"

# Check if VM already exists
if multipass list | grep -q "$VM_NAME"; then
    echo -e "${YELLOW}VM '$VM_NAME' already exists.${NC}"
    echo "Options:"
    echo "  1. Delete and recreate (fresh start)"
    echo "  2. Use existing VM"
    echo "  3. Cancel"
    read -p "Choose [1/2/3]: " choice

    case $choice in
        1)
            echo -e "${YELLOW}Deleting existing VM...${NC}"
            multipass delete "$VM_NAME"
            multipass purge
            ;;
        2)
            echo -e "${GREEN}Using existing VM${NC}"
            ;;
        3)
            echo "Cancelled"
            exit 0
            ;;
        *)
            echo "Invalid choice"
            exit 1
            ;;
    esac
fi

# Create VM if it doesn't exist
if ! multipass list | grep -q "$VM_NAME"; then
    echo -e "${BLUE}Creating Ubuntu 24.04 VM...${NC}"
    echo "  Name: $VM_NAME"
    echo "  Memory: 4GB"
    echo "  Disk: 20GB"
    echo ""

    multipass launch 24.04 \
        --name "$VM_NAME" \
        --memory 4G \
        --disk 20G \
        --cpus 2

    echo -e "${GREEN}✓ VM created successfully${NC}"
fi

# Transfer AudioForge source code
echo ""
echo -e "${BLUE}Transferring AudioForge source code...${NC}"

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
multipass transfer "$PROJECT_ROOT" "$VM_NAME:/home/ubuntu/"

echo -e "${GREEN}✓ Source code transferred${NC}"

# Install build dependencies
echo ""
echo -e "${BLUE}Installing build dependencies in VM...${NC}"

multipass exec "$VM_NAME" -- bash -c '
    sudo apt update
    sudo apt install -y \
        build-essential \
        cmake \
        git \
        dpkg-dev \
        rpm \
        wget \
        libasound2-dev \
        libfreetype6-dev \
        libx11-dev \
        libxrandr-dev \
        libxinerama-dev \
        libxcursor-dev \
        libcurl4-openssl-dev \
        mesa-common-dev
'

echo -e "${GREEN}✓ Dependencies installed${NC}"

# Build plugins
echo ""
echo -e "${BLUE}Building AudioForge plugins...${NC}"
echo "This may take 5-10 minutes..."
echo ""

multipass exec "$VM_NAME" -- bash -c '
    cd /home/ubuntu/AudioForge
    ./build-installer.sh
'

# Build Linux packages
echo ""
echo -e "${BLUE}Building Linux packages (.deb, .rpm, AppImage)...${NC}"

multipass exec "$VM_NAME" -- bash -c '
    cd /home/ubuntu/AudioForge/installers/linux
    chmod +x build-packages.sh
    ./build-packages.sh
'

# Copy packages back to macOS
echo ""
echo -e "${BLUE}Copying packages to macOS...${NC}"

OUTPUT_DIR="$PROJECT_ROOT/releases/installers"
mkdir -p "$OUTPUT_DIR"

multipass transfer "$VM_NAME:/home/ubuntu/AudioForge/releases/installers/"*.deb "$OUTPUT_DIR/" 2>/dev/null || true
multipass transfer "$VM_NAME:/home/ubuntu/AudioForge/releases/installers/"*.rpm "$OUTPUT_DIR/" 2>/dev/null || true
multipass transfer "$VM_NAME:/home/ubuntu/AudioForge/releases/installers/"*.AppImage "$OUTPUT_DIR/" 2>/dev/null || true

echo -e "${GREEN}✓ Packages copied${NC}"

# Summary
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Linux Packages Built Successfully!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Output directory: $OUTPUT_DIR"
echo ""
ls -lh "$OUTPUT_DIR"/*.{deb,rpm,AppImage} 2>/dev/null || echo "No packages found"
echo ""
echo "VM Commands:"
echo "  Shell into VM:    multipass shell $VM_NAME"
echo "  Stop VM:          multipass stop $VM_NAME"
echo "  Start VM:         multipass start $VM_NAME"
echo "  Delete VM:        multipass delete $VM_NAME && multipass purge"
