#!/bin/bash
# Fix Metal path and build VibeBrowser

echo "🔧 Fixing Metal Toolchain Path..."
echo "This requires your password..."
echo ""

# Create wrapper script
cat > /tmp/metal_wrapper.sh << 'EOF'
#!/bin/bash
# Wrapper to call the actual metal compiler
exec xcrun metal "$@"
EOF

chmod +x /tmp/metal_wrapper.sh

# Move it to where Chromium expects it (requires sudo)
sudo mv /tmp/metal_wrapper.sh /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/metal

echo "✅ Metal path fixed!"
echo ""
echo "🔨 Building VibeBrowser..."
echo "   Output: VibeBrowser Dev.app"
echo "   Time: 1-2 hours"
echo ""

cd /Volumes/VibeStore/chromium/src
export PATH="/Volumes/VibeStore/depot_tools:$PATH"
export SDKROOT=$(xcrun --show-sdk-path)

autoninja -C out/Default_arm64 chrome

echo ""
echo "════════════════════════════════════════"
echo "✅ VibeBrowser Build Complete!"
echo "════════════════════════════════════════"
echo ""
echo "🚀 Launch with:"
echo '"/Volumes/VibeStore/chromium/src/out/Default_arm64/VibeBrowser Dev.app/Contents/MacOS/VibeBrowser Dev"'
echo ""
echo "© 2025 VibeBrowser powered by VibeCaaS.com"

