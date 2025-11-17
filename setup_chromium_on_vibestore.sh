#!/bin/bash
# Setup Chromium on VibeStore external drive
# Run this script with: bash setup_chromium_on_vibestore.sh

set -e

echo "🌐 VibeBrowser - Chromium Setup on VibeStore"
echo "=============================================="
echo ""

# Fix permissions on VibeStore
echo "Step 1: Fixing permissions on VibeStore (needs sudo)..."
echo "You'll be asked for your password..."
sudo chown -R $(whoami):staff /Volumes/VibeStore/depot_tools/
sudo mkdir -p /Volumes/VibeStore/chromium
sudo chown -R $(whoami):staff /Volumes/VibeStore/chromium/
echo "✅ Permissions fixed"
echo ""

# Setup PATH
echo "Step 2: Setting up depot_tools PATH..."
export PATH="/Volumes/VibeStore/depot_tools:$PATH"
echo "✅ PATH configured"
echo ""

# Initialize depot_tools
echo "Step 3: Initializing depot_tools..."
cd /Volumes/VibeStore/depot_tools
gclient --version
echo "✅ depot_tools initialized"
echo ""

# Fetch Chromium
echo "Step 4: Fetching Chromium source (~100GB, 2-3 hours)..."
echo "⏳ This will take a while. Progress will be shown below."
echo ""
cd /Volumes/VibeStore/chromium
fetch chromium

echo ""
echo "✅ Chromium source downloaded!"
echo ""

# Checkout correct version
echo "Step 5: Checking out version 137.0.7187.69..."
cd /Volumes/VibeStore/chromium/src
git checkout 137.0.7187.69
echo "✅ Version checked out"
echo ""

# Sync dependencies
echo "Step 6: Syncing dependencies..."
gclient sync
echo "✅ Dependencies synced"
echo ""

# Install build dependencies
echo "Step 7: Installing build dependencies..."
./build/install-build-deps.sh
echo "✅ Build dependencies installed"
echo ""

# Run hooks
echo "Step 8: Running gclient hooks..."
gclient runhooks
echo "✅ Hooks completed"
echo ""

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║             ✅ CHROMIUM SETUP COMPLETE!                      ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""
echo "🚀 Ready to build VibeBrowser!"
echo ""
echo "Next steps:"
echo "  1. Add to your shell config:"
echo '     echo '"'"'export PATH="/Volumes/VibeStore/depot_tools:$PATH"'"'"' >> ~/.zshrc'
echo ""
echo "  2. Build VibeBrowser:"
echo "     cd /Users/knightdev/vibebrowser"
echo "     export CHROMIUM_SRC=/Volumes/VibeStore/chromium/src"
echo "     ./BUILD_VIBEBROWSER.sh"
echo ""
echo "© 2025 VibeBrowser powered by VibeCaaS.com a division of NeuralQuantum.ai LLC."

