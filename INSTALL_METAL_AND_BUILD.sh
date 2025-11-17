#!/bin/bash
# Install Metal Toolchain and Complete VibeBrowser Build

echo "🌐 VibeBrowser - Install Metal Toolchain and Build"
echo "===================================================="
echo ""
echo "⚠️  You'll need to enter your password for sudo commands"
echo ""

# Check if metal is already installed
if command -v metal &> /dev/null; then
    echo "✅ Metal Toolchain already installed"
else
    echo "📥 Installing Metal Toolchain..."
    echo "   This may take a few minutes..."
    sudo xcodebuild -runFirstLaunch
    echo "✅ Metal Toolchain installed"
fi

echo ""
echo "🔨 Building VibeBrowser..."
echo "   Output will be: VibeBrowser Dev.app"
echo "   Location: /Volumes/VibeStore/chromium/src/out/Default_arm64/"
echo ""
echo "   This will take 1-2 hours..."
echo "   Progress will be shown below"
echo ""

cd /Volumes/VibeStore/chromium/src
export PATH="/Volumes/VibeStore/depot_tools:$PATH"

# Build Chrome (will create VibeBrowser Dev.app)
autoninja -C out/Default_arm64 chrome

echo ""
echo "════════════════════════════════════════════"
echo "✅ VibeBrowser Build Complete!"
echo "════════════════════════════════════════════"
echo ""
echo "🚀 Launch VibeBrowser with:"
echo '   "/Volumes/VibeStore/chromium/src/out/Default_arm64/VibeBrowser Dev.app/Contents/MacOS/VibeBrowser Dev"'
echo ""
echo "Or double-click the app in Finder:"
echo "   /Volumes/VibeStore/chromium/src/out/Default_arm64/VibeBrowser Dev.app"
echo ""
echo "© 2025 VibeBrowser powered by VibeCaaS.com a division of NeuralQuantum.ai LLC."

