# 🔧 Build VibeBrowser Using VibeStore External Drive

## Setup Guide

You want to use `/Volumes/VibeStore/` for depot_tools and Chromium to save space on your main drive.

---

## ✅ Quick Setup (Automated)

### Run the Setup Script

```bash
cd /Users/knightdev/vibebrowser
bash setup_chromium_on_vibestore.sh
```

This script will:
1. Fix permissions on VibeStore (asks for sudo password)
2. Initialize depot_tools
3. Download Chromium (~100GB, 2-3 hours)
4. Sync dependencies
5. Install build tools
6. Run hooks

**Total time:** ~2-3 hours

---

## 🔧 Manual Setup (Step-by-Step)

If you prefer manual control:

### Step 1: Fix Permissions
```bash
# You'll need to enter your password
sudo chown -R $(whoami):staff /Volumes/VibeStore/depot_tools/
sudo mkdir -p /Volumes/VibeStore/chromium
sudo chown -R $(whoami):staff /Volumes/VibeStore/chromium/
```

### Step 2: Setup PATH
```bash
export PATH="/Volumes/VibeStore/depot_tools:$PATH"

# Add to your ~/.zshrc permanently:
echo 'export PATH="/Volumes/VibeStore/depot_tools:$PATH"' >> ~/.zshrc
```

### Step 3: Initialize depot_tools
```bash
cd /Volumes/VibeStore/depot_tools
gclient --version
# This creates .cipd_bin and downloads necessary tools
```

### Step 4: Fetch Chromium (~100GB, 2-3 hours)
```bash
cd /Volumes/VibeStore/chromium
fetch chromium

# ⏳ This is the long step - grab coffee!
# Progress will be shown as it downloads
```

### Step 5: Checkout Version
```bash
cd /Volumes/VibeStore/chromium/src
git checkout 137.0.7187.69
```

### Step 6: Sync Dependencies
```bash
gclient sync
```

### Step 7: Install Build Dependencies
```bash
./build/install-build-deps.sh
```

### Step 8: Run Hooks
```bash
gclient runhooks
```

---

## 🚀 Building VibeBrowser

### After Chromium Setup Completes

```bash
cd /Users/knightdev/vibebrowser/packages/vibebrowser
source venv/bin/activate

# Build with VibeStore Chromium
python build/build.py \
  --config build/config/debug.yaml \
  --chromium-src /Volumes/VibeStore/chromium/src \
  --build
```

**OR use the automated script:**
```bash
cd /Users/knightdev/vibebrowser
export CHROMIUM_SRC=/Volumes/VibeStore/chromium/src
./BUILD_VIBEBROWSER.sh
```

---

## 📍 File Locations

```
depot_tools:    /Volumes/VibeStore/depot_tools/
Chromium source: /Volumes/VibeStore/chromium/src/
Build output:    /Volumes/VibeStore/chromium/src/out/
VibeBrowser app: /Volumes/VibeStore/chromium/src/out/Default_arm64/VibeBrowser Dev.app
```

---

## 💾 Disk Space

### VibeStore
```
Available: 3.5TB ✅ (plenty of space!)
Needed:    ~120GB
  • Chromium source: ~100GB
  • Build output: ~20GB
```

### Advantages of Using External Drive
✅ Saves main drive space
✅ 3.5TB available (no worries!)
✅ Can move to other machines
✅ Easy to backup

---

## ⏱️ Time Estimates

| Phase | Time | What Happens |
|-------|------|--------------|
| Fix permissions | 1 min | sudo commands |
| Initialize depot_tools | 2 mins | Download tools |
| **Fetch Chromium** | **2-3 hours** | **~100GB download** |
| Sync dependencies | 30 mins | Additional files |
| Install build deps | 10 mins | System packages |
| Run hooks | 15 mins | Setup scripts |
| **Build VibeBrowser** | **1-3 hours** | **Compilation** |
| **TOTAL** | **4-6 hours** | **One-time setup** |

Future rebuilds: Only 5-30 minutes! ⚡

---

## 🔍 Monitoring Progress

### Check Download Progress
```bash
# Watch live output
tail -f /Users/knightdev/.cursor/projects/Users-knightdev-vibebrowser/terminals/11.txt

# Check size growing
watch -n 60 du -sh /Volumes/VibeStore/chromium

# Check network activity
nettop -m tcp
```

### Expected Milestones
```
5 mins:   .gclient created
10 mins:  src/ directory appears  
30 mins:  ~10GB downloaded
1 hour:   ~30GB downloaded
2 hours:  ~60GB downloaded
3 hours:  ~100GB - COMPLETE! ✅
```

---

## 🛑 Troubleshooting

### "Permission denied" on VibeStore
```bash
sudo chown -R $(whoami):staff /Volumes/VibeStore/depot_tools/
sudo chown -R $(whoami):staff /Volumes/VibeStore/chromium/
```

### "cipd_bin not found"
```bash
cd /Volumes/VibeStore/depot_tools
export PATH="/Volumes/VibeStore/depot_tools:$PATH"
gclient --version  # This initializes cipd
```

### "Out of disk space"
```bash
# Check space
df -h /Volumes/VibeStore

# You have 3.5TB, so this shouldn't happen!
```

### Download interrupted
```bash
# Resume the download
cd /Volumes/VibeStore/chromium
gclient sync
```

---

## ✅ After Setup

### Verify Chromium is Ready
```bash
ls -la /Volumes/VibeStore/chromium/src/
# Should show thousands of files

du -sh /Volumes/VibeStore/chromium/src/
# Should be ~100GB
```

### Build VibeBrowser
```bash
cd /Users/knightdev/vibebrowser
export PATH="/Volumes/VibeStore/depot_tools:$PATH"
export CHROMIUM_SRC=/Volumes/VibeStore/chromium/src

./BUILD_VIBEBROWSER.sh
```

---

## 🎯 Summary

**Location:** Using /Volumes/VibeStore/ (3.5TB available) ✅
**Steps:** Run `setup_chromium_on_vibestore.sh` or follow manual steps
**Time:** ~2-3 hours for Chromium download
**Result:** VibeBrowser.app ready to build!

---

© 2025 VibeBrowser powered by VibeCaaS.com a division of NeuralQuantum.ai LLC. All rights reserved.

