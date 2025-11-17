#!/bin/bash
# VibeBrowser Build Script
# This script builds VibeBrowser from Chromium source with all the new branding

set -e

echo "🌐 VibeBrowser Build Script"
echo "============================"
echo ""

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect OS
OS_TYPE="$(uname -s)"
case "${OS_TYPE}" in
    Linux*)     PLATFORM=linux;;
    Darwin*)    PLATFORM=macos;;
    CYGWIN*)    PLATFORM=windows;;
    MINGW*)     PLATFORM=windows;;
    *)          PLATFORM=unknown;;
esac

echo -e "${BLUE}Detected platform: ${PLATFORM}${NC}"
echo ""

# Check if Chromium source exists
CHROMIUM_SRC="${CHROMIUM_SRC:-$HOME/chromium/src}"

if [ ! -d "$CHROMIUM_SRC" ]; then
    echo -e "${RED}❌ Chromium source not found at: $CHROMIUM_SRC${NC}"
    echo ""
    echo "Please set up Chromium source first:"
    echo ""
    echo "  mkdir ~/chromium && cd ~/chromium"
    echo "  fetch chromium"
    echo "  cd src"
    echo "  git checkout 137.0.7151.69"
    echo "  gclient sync"
    if [ "$PLATFORM" = "linux" ]; then
        echo "  ./build/install-build-deps.sh"
    fi
    echo "  gclient runhooks"
    echo ""
    echo "Then run this script again, or set CHROMIUM_SRC environment variable:"
    echo "  export CHROMIUM_SRC=~/chromium/src"
    echo "  ./BUILD_VIBEBROWSER.sh"
    exit 1
fi

echo -e "${GREEN}✅ Found Chromium source at: $CHROMIUM_SRC${NC}"
echo ""

# Navigate to build directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}/packages/vibebrowser"

# Check if build system exists
if [ ! -d "build" ]; then
    echo -e "${RED}❌ Build system not found${NC}"
    echo ""
    echo "The Python build scripts are not yet available in this repository."
    echo "Please refer to CHROMIUM_SETUP_NEEDED.md for manual build instructions."
    echo ""
    exit 1
fi

# Activate virtual environment
if [ -d "venv" ]; then
    echo -e "${GREEN}✅ Activating Python virtual environment${NC}"
    source venv/bin/activate
else
    echo -e "${YELLOW}⚠️  Virtual environment not found, creating...${NC}"
    python3 -m venv venv
    source venv/bin/activate
    pip install -r requirements.txt
fi

echo ""
echo "🔨 Building VibeBrowser..."
echo ""
echo "Build type: ${BUILD_TYPE:-debug}"
echo "Architecture: ${ARCH:-arm64}"
echo "Chromium source: $CHROMIUM_SRC"
echo ""

# Determine config file based on platform and build type
if [ "$BUILD_TYPE" = "release" ]; then
    if [ "$PLATFORM" = "linux" ]; then
        CONFIG_FILE="${CONFIG_FILE:-build/config/release.linux.yaml}"
    elif [ "$PLATFORM" = "macos" ]; then
        CONFIG_FILE="${CONFIG_FILE:-build/config/release.macos.yaml}"
    else
        CONFIG_FILE="${CONFIG_FILE:-build/config/release.yaml}"
    fi
else
    if [ "$PLATFORM" = "linux" ]; then
        CONFIG_FILE="${CONFIG_FILE:-build/config/debug.linux.yaml}"
    elif [ "$PLATFORM" = "macos" ]; then
        CONFIG_FILE="${CONFIG_FILE:-build/config/debug.macos.yaml}"
    else
        CONFIG_FILE="${CONFIG_FILE:-build/config/debug.yaml}"
    fi
fi

echo "Using config: $CONFIG_FILE"
echo ""
echo -e "${YELLOW}⏳ This will take 1-3 hours depending on your system...${NC}"
echo ""

# Run build
python build/build.py \
    --config "$CONFIG_FILE" \
    --chromium-src "$CHROMIUM_SRC" \
    --build

echo ""
echo -e "${GREEN}✅ Build complete!${NC}"
echo ""
echo "🚀 To run VibeBrowser:"
echo ""

if [ "$PLATFORM" = "linux" ]; then
    if [ "$BUILD_TYPE" = "release" ]; then
        echo "  $CHROMIUM_SRC/out/Default/vibebrowser"
    else
        echo "  $CHROMIUM_SRC/out/Debug/vibebrowser"
    fi
elif [ "$PLATFORM" = "macos" ]; then
    if [ "$BUILD_TYPE" = "release" ]; then
        echo "  $CHROMIUM_SRC/out/Default_arm64/VibeBrowser.app/Contents/MacOS/VibeBrowser"
    else
        echo "  $CHROMIUM_SRC/out/Default_arm64/VibeBrowser\ Dev.app/Contents/MacOS/VibeBrowser\ Dev"
    fi
else
    echo "  Check the out/ directory for built binaries"
fi

echo ""
echo "© 2025 VibeBrowser powered by VibeCaaS.com a division of NeuralQuantum.ai LLC."

