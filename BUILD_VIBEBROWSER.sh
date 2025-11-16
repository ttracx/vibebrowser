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
NC='\033[0m' # No Color

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
    echo "  git checkout 137.0.7187.69"
    echo "  gclient sync"
    echo "  ./build/install-build-deps.sh"
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
cd "$(dirname "$0")/packages/vibebrowser"

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

# Determine config file
CONFIG_FILE="${CONFIG_FILE:-build/config/debug.yaml}"

if [ "$BUILD_TYPE" = "release" ]; then
    CONFIG_FILE="build/config/release.macos.yaml"
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

if [ "$BUILD_TYPE" = "release" ]; then
    echo "  $CHROMIUM_SRC/out/Default_arm64/VibeBrowser.app/Contents/MacOS/VibeBrowser"
else
    echo "  $CHROMIUM_SRC/out/Default_arm64/VibeBrowser\ Dev.app/Contents/MacOS/VibeBrowser\ Dev"
fi

echo ""
echo "© 2025 VibeBrowser powered by VibeCaaS.com a division of NeuralQuantum.ai LLC."

