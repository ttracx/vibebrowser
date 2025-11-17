#!/bin/bash
# Fix all Metal toolchain paths for VibeBrowser build

echo "🔧 Fixing Metal Toolchain Paths..."
echo "This requires your password..."
echo ""

# Create metal wrapper
cat > /tmp/metal_wrapper.sh << 'EOF'
#!/bin/bash
exec xcrun metal "$@"
EOF

# Create metallib wrapper
cat > /tmp/metallib_wrapper.sh << 'EOF'
#!/bin/bash
exec xcrun metallib "$@"
EOF

chmod +x /tmp/metal_wrapper.sh
chmod +x /tmp/metallib_wrapper.sh

# Install wrappers (requires sudo)
sudo mv /tmp/metal_wrapper.sh /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/metal
sudo mv /tmp/metallib_wrapper.sh /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/metallib

echo "✅ Metal toolchain paths fixed!"
echo ""
echo "🔨 Building VibeBrowser..."
echo "   Output: VibeBrowser Dev.app"
echo "   Location: /Volumes/VibeStore/chromium/src/out/Default_arm64/"
echo "   Time: 1-2 hours"
echo ""

cd /Volumes/VibeStore/chromium/src
export PATH="/Volumes/VibeStore/depot_tools:$PATH"
export SDKROOT=$(xcrun --show-sdk-path)

autoninja -C out/Default_arm64 chrome

echo ""
echo "════════════════════════════════════════════════"
echo "✅ VibeBrowser Build Complete!"
echo "════════════════════════════════════════════════"
echo ""
echo "🚀 Launch VibeBrowser with:"
echo '   open "/Volumes/VibeStore/chromium/src/out/Default_arm64/VibeBrowser Dev.app"'
echo ""
echo "Or run directly:"
echo '   "/Volumes/VibeStore/chromium/src/out/Default_arm64/VibeBrowser Dev.app/Contents/MacOS/VibeBrowser Dev"'
echo ""
echo "© 2025 VibeBrowser powered by VibeCaaS.com a division of NeuralQuantum.ai LLC."

