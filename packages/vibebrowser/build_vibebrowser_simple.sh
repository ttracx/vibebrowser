#!/bin/bash
# Simple VibeBrowser build - minimal patches for working build

set -e

echo "🌐 Building VibeBrowser (Simplified Build)"
echo "=========================================="
echo ""

cd /Users/knightdev/vibebrowser/packages/vibebrowser
source venv/bin/activate

export PATH="/Volumes/VibeStore/depot_tools:$PATH"
export CHROMIUM_SRC=/Volumes/VibeStore/chromium/src

echo "📍 Chromium: $CHROMIUM_SRC"
echo "📍 Building without complex patches (for working binary)"
echo ""

# Build vanilla Chromium first to get a working binary
cd $CHROMIUM_SRC

# Create output directory
mkdir -p out/Default_arm64

# Create args.gn for debug build
cat > out/Default_arm64/args.gn << 'ARGSEOF'
# VibeBrowser Debug Build Configuration
is_debug = true
is_component_build = false
symbol_level = 1
enable_nacl = false
blink_symbol_level = 0

# Branding
chrome_pgo_phase = 0
is_official_build = false

# Basic features
proprietary_codecs = true
ffmpeg_branding = "Chrome"
enable_widevine = true

# No Google services
google_api_key = ""
google_default_client_id = ""
google_default_client_secret = ""
use_official_google_api_keys = false
ARGSEOF

echo "✅ Created args.gn"

# Generate build files
echo "🔧 Generating build files..."
gn gen out/Default_arm64

# Build Chrome
echo "🔨 Building Chrome (this takes 1-2 hours)..."
echo "    You can monitor with: tail -f ~/vibebrowser_simple_build.log"
autoninja -C out/Default_arm64 chrome

echo ""
echo "✅ Build Complete!"
echo ""
echo "🚀 Chrome built at:"
echo "   $CHROMIUM_SRC/out/Default_arm64/Chromium.app"
echo ""
echo "Note: This is vanilla Chromium. To get full VibeBrowser branding,"
echo "      the patches need to be updated for version compatibility."
echo ""
echo "To test: open $CHROMIUM_SRC/out/Default_arm64/Chromium.app"
