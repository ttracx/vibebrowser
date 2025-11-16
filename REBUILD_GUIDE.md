# VibeBrowser Rebuild Guide

## Current Status

✅ **Completed Steps:**
1. ✅ Updated all documentation with VibeBrowser branding
2. ✅ Created VibeCaaS theme system (`docs/vibecaas-theme.css`)
3. ✅ Updated `docs.json` with new colors and custom CSS import
4. ✅ Added copyright footer to all documentation pages
5. ✅ Updated build configuration files with new name
6. ✅ BrowserOS.app is currently running (temporary name)

⚠️ **Important Note:** 
Simply renaming the app bundle breaks code signing. To fully rebrand to VibeBrowser, you need to rebuild from Chromium source.

## Prerequisites for Full Rebuild

### System Requirements
- macOS (M1/M2/M3/M4)
- ~100GB free disk space
- 16GB+ RAM (recommended)
- 3+ hours for first build
- Xcode Command Line Tools

### Required Tools
```bash
# Install depot_tools for Chromium
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH="$PATH:${HOME}/depot_tools"
```

## Step-by-Step Rebuild Process

### 1. Setup Chromium Source (First Time Only - ~2-3 hours)

```bash
# Create chromium directory
mkdir -p ~/chromium
cd ~/chromium

# Fetch Chromium source (this downloads ~100GB)
fetch --nohooks chromium

# Sync specific version (matching CHROMIUM_VERSION file)
cd src
git checkout 137.0.7187.69
gclient sync

# Install dependencies
./build/install-build-deps.sh
gclient runhooks
```

### 2. Apply VibeBrowser Patches and Build

```bash
# Navigate to VibeBrowser build system
cd /Users/knightdev/vibebrowser/packages/vibebrowser

# Activate virtual environment
source venv/bin/activate

# Run debug build (faster, ~1-2 hours)
python build/build.py \
  --config build/config/debug.yaml \
  --chromium-src ~/chromium/src \
  --build

# OR run release build (slower, ~2-3 hours, but production-ready)
python build/build.py \
  --config build/config/release.macos.yaml \
  --chromium-src ~/chromium/src \
  --build
```

### 3. Sign and Package (Release Only)

```bash
# Set signing environment variables
export MACOS_CERTIFICATE_NAME="Your Certificate Name"
export PROD_MACOS_NOTARIZATION_APPLE_ID="your-apple-id@example.com"
export PROD_MACOS_NOTARIZATION_TEAM_ID="YOUR_TEAM_ID"
export PROD_MACOS_NOTARIZATION_PWD="app-specific-password"

# Build with signing and packaging
python build/build.py \
  --config build/config/release.macos.yaml \
  --chromium-src ~/chromium/src \
  --build \
  --sign \
  --package
```

### 4. Run VibeBrowser

After successful build:

**Debug Build (ARM64):**
```bash
~/chromium/src/out/Default_arm64/VibeBrowser\ Dev.app/Contents/MacOS/VibeBrowser\ Dev
```

**Release Build (Universal):**
```bash
~/chromium/src/out/Default_universal/VibeBrowser.app/Contents/MacOS/VibeBrowser
```

## Build System Updates Completed

### Configuration Files Updated:
- ✅ `build/config/copy_resources.yaml` - VibeBrowser branding
- ✅ `build/config/debug.yaml` - VibeBrowser branding  
- ✅ `build/config/release.macos.yaml` - VibeBrowser branding
- ✅ Icons and binaries are already named VibeBrowser in `resources/`

### Chromium Patches (Ready for Build):
The following patch files reference "Nxtscape" and will apply the branding during build:
- `chromium_patches/chrome/browser/extensions/api/settings_private/prefs_util.cc`
- `chromium_patches/chrome/browser/resources/settings/nxtscape_page/*`
- Various UI and preference files

**Note:** These patches are applied automatically during the build process and will create the VibeBrowser brand in the compiled application.

## Alternative: Quick Test (Current Approach)

Since a full rebuild requires significant time and resources, we've:
1. ✅ Updated all documentation and theme system
2. ✅ BrowserOS.app is running (contains VibeBrowser code, just with old bundle name)
3. ✅ All source code ready for proper rebuild

The currently running app is the VibeBrowser codebase, just with the temporary "BrowserOS" bundle identifier for testing.

## Troubleshooting

### "Chromium source directory not found"
- Make sure you've run `fetch chromium` and have `~/chromium/src`
- Check the path in config files matches your Chromium location

### "Build fails with missing dependencies"
- Run `gclient sync` in Chromium source directory
- Install Xcode Command Line Tools: `xcode-select --install`

### "Out of disk space"
- Chromium requires ~100GB. Free up space before building.
- Use `df -h` to check available space

### "Code signing fails"
- Set all required environment variables
- Use a valid Apple Developer certificate
- Generate app-specific password for notarization

## What's Been Applied

✅ **Documentation**: All `.mdx` files updated with VibeBrowser branding and new theme
✅ **Theme System**: Complete VibeCaaS theme with brand colors
✅ **Build Configs**: Updated all YAML files with VibeBrowser references
✅ **Logo Assets**: Using the provided `favicon.svg`, `light.svg`, and `vibebrowser-original.svg`

## Next Steps

**For Production Deployment:**
1. Complete Chromium source setup (if not done)
2. Run full release build with signing
3. Test the built VibeBrowser.app
4. Distribute the signed application

**For Development:**
1. Use debug build for faster iteration
2. Make code changes in `chromium_patches/`
3. Rebuild only changed files (incremental build is faster)

---
© 2025 VibeBrowser powered by VibeCaaS.com a division of NeuralQuantum.ai LLC. All rights reserved.

