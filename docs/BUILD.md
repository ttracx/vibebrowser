# Building VibeBrowser from Source

This guide covers how to build VibeBrowser from Chromium source code.

## Overview

VibeBrowser is a custom Chromium build with additional features and branding. Building requires:
- Chromium source code (~100GB)
- Build tools (depot_tools, compiler, etc.)
- 2-6 hours for the first build
- 16GB+ RAM recommended

## Prerequisites

### 1. System Requirements

**Disk Space:** 120GB+ free space
- Chromium source: ~100GB
- Build output: ~20GB
- VibeBrowser app: ~200MB

**RAM:** 16GB+ recommended (8GB minimum)

**OS Support:**
- macOS 11.0+ (Big Sur or later)
- Ubuntu 20.04+ or similar Linux
- Windows 10+ with Visual Studio 2022

### 2. Install Build Tools

#### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install depot_tools
cd ~
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH="$PATH:${HOME}/depot_tools"

# Add to shell profile for persistence
echo 'export PATH="$PATH:${HOME}/depot_tools"' >> ~/.zshrc
source ~/.zshrc
```

#### Linux

```bash
# Install build dependencies
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  git \
  python3 \
  python3-pip \
  curl

# Install depot_tools
cd ~
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH="$PATH:${HOME}/depot_tools"

# Add to shell profile
echo 'export PATH="$PATH:${HOME}/depot_tools"' >> ~/.bashrc
source ~/.bashrc
```

#### Windows

```bash
# Install Visual Studio 2022 with:
# - Desktop development with C++
# - Windows 10 SDK
# - ATL/MFC libraries

# Install depot_tools
# Download from: https://storage.googleapis.com/chrome-infra/depot_tools.zip
# Extract to C:\depot_tools
# Add to PATH in System Environment Variables
```

### 3. Fetch Chromium Source

This is a **one-time** download of ~100GB that takes 2-3 hours:

```bash
# Create directory
mkdir -p ~/chromium
cd ~/chromium

# Fetch Chromium (this downloads ~100GB)
fetch chromium

# This will take 2-3 hours depending on internet speed
# Progress will be shown as it downloads
```

### 4. Checkout Correct Version

```bash
cd ~/chromium/src

# Checkout VibeBrowser's Chromium version
git checkout 137.0.7187.69

# Sync dependencies
gclient sync

# Install build dependencies (macOS)
./build/install-build-deps.sh

# Run hooks
gclient runhooks
```

### 5. Install Python Dependencies

```bash
cd /path/to/vibebrowser/packages/vibebrowser
pip install -r requirements.txt
```

## Check Prerequisites

Before building, verify everything is set up:

```bash
cd packages/vibebrowser
python build/check_prerequisites.py --chromium-src ~/chromium/src
```

This will check:
- Python version (3.8+ required)
- Required tools (git, gn, ninja)
- Python packages (PyYAML)
- Chromium source availability
- Disk space
- Platform-specific tools

## Building VibeBrowser

### Quick Build (Automated)

Use the automated build script:

```bash
# From the repository root
./BUILD_VIBEBROWSER.sh

# Or with custom Chromium path
CHROMIUM_SRC=/custom/path/chromium/src ./BUILD_VIBEBROWSER.sh

# Or for release build
BUILD_TYPE=release ./BUILD_VIBEBROWSER.sh
```

### Manual Build

#### Debug Build (Recommended for Development)

Debug builds are faster (~1-2 hours) and include debug symbols:

```bash
cd packages/vibebrowser

# macOS
python build/build.py \
  --config build/config/debug.macos.yaml \
  --chromium-src ~/chromium/src \
  --build

# Linux
python build/build.py \
  --config build/config/debug.linux.yaml \
  --chromium-src ~/chromium/src \
  --build

# Windows
python build/build.py \
  --config build/config/debug.windows.yaml \
  --chromium-src ~/chromium/src \
  --build
```

#### Release Build (Production)

Release builds are optimized (~2-3 hours) but slower to compile:

```bash
cd packages/vibebrowser

# macOS
python build/build.py \
  --config build/config/release.macos.yaml \
  --chromium-src ~/chromium/src \
  --build

# Linux
python build/build.py \
  --config build/config/release.linux.yaml \
  --chromium-src ~/chromium/src \
  --build

# Windows
python build/build.py \
  --config build/config/release.windows.yaml \
  --chromium-src ~/chromium/src \
  --build
```

### Build Options

#### Full Build
```bash
python build/build.py --config CONFIG --chromium-src PATH --build
```

#### Apply Patches Only
```bash
python build/build.py --config CONFIG --chromium-src PATH --patches-only
```

#### Configure Only (Skip Compilation)
```bash
python build/build.py --config CONFIG --chromium-src PATH --configure-only
```

#### Verbose Output
```bash
python build/build.py --config CONFIG --chromium-src PATH --build --verbose
```

## Build Output

After building, find your compiled application:

### macOS
```bash
# Debug build
~/chromium/src/out/Default_arm64/VibeBrowser Dev.app

# Release build
~/chromium/src/out/Default_arm64/VibeBrowser.app

# Run it
~/chromium/src/out/Default_arm64/VibeBrowser.app/Contents/MacOS/VibeBrowser
```

### Linux
```bash
# Debug or Release
~/chromium/src/out/Default_x64/vibebrowser

# Run it
~/chromium/src/out/Default_x64/vibebrowser
```

### Windows
```bash
# Debug or Release
~/chromium/src/out/Default_x64/vibebrowser.exe

# Run it
~/chromium/src/out/Default_x64/vibebrowser.exe
```

## Build Times

| Phase | First Time | Subsequent |
|-------|-----------|-----------|
| Download Chromium | 2-3 hours | - |
| Sync dependencies | 30 mins | 5 mins |
| Debug build | 1-2 hours | 5-30 mins |
| Release build | 2-3 hours | 30-60 mins |

**Total first-time:** 4-6 hours  
**Incremental builds:** 5-60 minutes

## Troubleshooting

### "Chromium source not found"

Make sure Chromium is properly checked out:
```bash
ls ~/chromium/src/.gclient  # Should exist
```

If not, fetch it:
```bash
cd ~/chromium
fetch chromium
```

### "Patch already applied or has conflicts"

Your Chromium source may already have patches applied. Reset it:
```bash
git -C ~/chromium/src reset --hard
gclient sync
```

### "GN configuration failed"

Ensure depot_tools is in your PATH:
```bash
which gn     # Should show path to gn
which ninja  # Should show path to ninja
```

Add depot_tools to PATH:
```bash
export PATH="$PATH:${HOME}/depot_tools"
```

### Build fails with missing dependencies

Run the dependency installer:
```bash
# macOS/Linux
cd ~/chromium/src
./build/install-build-deps.sh
gclient runhooks

# Windows
# Re-run Visual Studio installer and ensure all C++ components are installed
```

### Out of disk space

Chromium builds need significant space. Clean old builds:
```bash
# Remove old build artifacts
rm -rf ~/chromium/src/out/Default_*

# Or use gn clean
cd ~/chromium/src
gn clean out/Default_arm64
```

### Build is very slow

Tips for faster builds:
1. Use debug builds for development (faster compilation)
2. Use component builds (enabled in debug configs)
3. Close other applications to free RAM
4. Consider using `ccache` for caching
5. Ensure you have 16GB+ RAM

### macOS: Code signing issues

Debug builds use ad-hoc signing by default. For distribution:
1. Get an Apple Developer account
2. Create a Developer ID certificate
3. Update `release.macos.yaml` with your signing identity

## Incremental Builds

After the first build, subsequent builds are much faster:

```bash
# Make your changes to VibeBrowser source in:
# - packages/vibebrowser/chromium_patches/
# - packages/vibebrowser/patches/

# Then rebuild (only changed files recompile)
python build/build.py \
  --config build/config/debug.macos.yaml \
  --chromium-src ~/chromium/src \
  --build

# Or just compile without reapplying patches
cd ~/chromium/src
ninja -C out/Default_arm64 chrome
```

## Configuration

Build configurations are in `packages/vibebrowser/build/config/`:

- `debug.macos.yaml` - macOS debug build
- `debug.linux.yaml` - Linux debug build
- `debug.windows.yaml` - Windows debug build
- `release.macos.yaml` - macOS release build
- `release.linux.yaml` - Linux release build
- `release.windows.yaml` - Windows release build

You can customize these or create your own. See `build/README.md` for details.

## Advanced Topics

### Using Component Builds

Component builds split the browser into DLLs/dylibs for faster linking:
```yaml
# In your config file
gn_args:
  is_component_build: true  # Faster incremental builds
```

### Using ccache

Speed up recompilation with ccache:
```bash
# Install ccache
brew install ccache  # macOS
sudo apt-get install ccache  # Linux

# Configure GN
# Add to your config's gn_args:
cc_wrapper: "ccache"
```

### Cross-compilation

Build for different architectures:
```yaml
# In config file
target_cpu: "arm64"  # For Apple Silicon
target_cpu: "x64"    # For Intel/AMD
```

### Debug Symbols

Control symbol generation:
```yaml
gn_args:
  symbol_level: 0  # No symbols (smallest binary)
  symbol_level: 1  # Minimal symbols
  symbol_level: 2  # Full symbols (largest, best debugging)
```

## Support

If you run into issues:

1. Check this guide and `build/README.md`
2. Run the prerequisites checker
3. Read [Chromium's build documentation](https://www.chromium.org/developers/how-tos/get-the-code/)
4. Ask on our [Discord](https://discord.gg/YKwjt5vuKr)
5. Open an issue on [GitHub](https://github.com/VibeBrowser-ai/VibeBrowser/issues)

## Next Steps

After building:
1. Test your build
2. Make changes to VibeBrowser features
3. Rebuild incrementally
4. Submit a pull request!

See [CONTRIBUTING.md](../CONTRIBUTING.md) for contribution guidelines.

---

© 2025 VibeBrowser powered by VibeCaaS.com a division of NeuralQuantum.ai LLC. All rights reserved.
