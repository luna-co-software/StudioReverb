#!/bin/bash

# Complete setup script for StudioReverb
set -e  # Exit on error

echo "==============================================="
echo "Complete Setup for StudioReverb"
echo "==============================================="
echo ""

# 1. Clean up old structure
echo "Step 1: Cleaning up old structure..."
if [ -d "plugins/studio-reverb" ]; then
    # Copy source files if they don't exist in root
    [ ! -f "DSP.cpp" ] && [ -f "plugins/studio-reverb/DSP.cpp" ] && cp plugins/studio-reverb/DSP.cpp .
    [ ! -f "UI.cpp" ] && [ -f "plugins/studio-reverb/UI.cpp" ] && cp plugins/studio-reverb/UI.cpp .

    # Remove plugins directory
    rm -rf plugins/
    echo "  ✓ Cleaned up old structure"
else
    echo "  ✓ Already clean"
fi

# 2. Initialize git repository
echo ""
echo "Step 2: Initializing git repository..."
if [ ! -d ".git" ]; then
    git init
    echo "  ✓ Git repository initialized"
else
    echo "  ✓ Git repository already initialized"
fi

# 3. Add remote origin
echo ""
echo "Step 3: Adding remote origin..."
if ! git remote | grep -q origin; then
    git remote add origin https://github.com/luna-co-software/StudioReverb.git
    echo "  ✓ Remote origin added"
else
    echo "  ✓ Remote origin already exists"
fi

# 4. Add DPF as submodule
echo ""
echo "Step 4: Adding DPF as git submodule..."
if [ ! -d "dpf" ]; then
    git submodule add https://github.com/DISTRHO/DPF.git dpf
    git submodule update --init --recursive
    echo "  ✓ DPF submodule added"
else
    echo "  ✓ DPF already exists"
    git submodule update --init --recursive
fi

# 5. Copy Freeverb3 library files
echo ""
echo "Step 5: Setting up Freeverb3 library..."
mkdir -p common/freeverb

# Check if we can copy from the original location
SOURCE_DIR="/home/marc/projects/plugins/plugins/StudioReverbDPF/common/freeverb"
if [ -d "$SOURCE_DIR" ]; then
    echo "  Copying from $SOURCE_DIR..."
    cp -n $SOURCE_DIR/*.hpp common/freeverb/ 2>/dev/null || true
    cp -n $SOURCE_DIR/*.cpp common/freeverb/ 2>/dev/null || true
    cp -n $SOURCE_DIR/*.h common/freeverb/ 2>/dev/null || true
    cp -n $SOURCE_DIR/COPYING common/freeverb/ 2>/dev/null || true
    cp -n $SOURCE_DIR/AUTHORS common/freeverb/ 2>/dev/null || true
    echo "  ✓ Freeverb3 files copied"
else
    echo "  ⚠ Source directory not found. You'll need to manually add Freeverb3 files to common/freeverb/"
    echo "    Required files: earlyref.*, progenitor2.*, nrevb.*, and supporting files"
fi

# 6. Verify all source files are present
echo ""
echo "Step 6: Verifying source files..."
missing_files=()
for file in DistrhoPluginInfo.h Plugin.cpp DSP.cpp DSP.hpp UI.cpp UI.hpp; do
    if [ -f "$file" ]; then
        echo "  ✓ $file"
    else
        echo "  ✗ $file (missing)"
        missing_files+=("$file")
    fi
done

if [ ${#missing_files[@]} -gt 0 ]; then
    echo ""
    echo "  ⚠ Warning: Some source files are missing!"
    echo "    The build will fail without these files."
fi

# 7. Try to build
echo ""
echo "Step 7: Building the plugin..."
if [ -f "dpf/Makefile.plugins.mk" ]; then
    make clean 2>/dev/null || true
    if make -j4; then
        echo "  ✓ Build successful!"
        echo ""
        echo "  Binaries created in:"
        ls -la bin/ 2>/dev/null | grep -E "\.lv2|\.vst|\.so" | head -5
    else
        echo "  ✗ Build failed. Check error messages above."
        echo ""
        echo "  Common issues:"
        echo "  - Missing Freeverb3 files in common/freeverb/"
        echo "  - Missing source files (DSP.cpp, UI.cpp)"
        echo "  - DPF not properly initialized"
    fi
else
    echo "  ✗ Cannot build - DPF not properly set up"
fi

# 8. Prepare for git commit
echo ""
echo "Step 8: Preparing git commit..."
git add .
echo "  ✓ Files staged for commit"

# 9. Create commit
echo ""
echo "Step 9: Creating initial commit..."
git commit -m "Initial commit: Studio Reverb - Unified Dragonfly Reverb plugin

- Combines 4 reverb algorithms (Room, Hall, Plate, Early Reflections)
- Dynamic UI that adapts to show relevant controls per algorithm
- Based on Freeverb3 DSP library
- Built with DPF (DISTRHO Plugin Framework)
- Supports LV2, VST2, and VST3 formats" || echo "  ⚠ Commit failed (maybe no changes to commit?)"

# 10. Final instructions
echo ""
echo "==============================================="
echo "Setup Complete!"
echo "==============================================="
echo ""
echo "To push to GitHub:"
echo "  git branch -M main"
echo "  git push -u origin main"
echo ""
echo "If the repository doesn't exist on GitHub yet:"
echo "  1. Go to https://github.com/new"
echo "  2. Create a new repository named 'StudioReverb'"
echo "  3. Make it public"
echo "  4. Don't initialize with README"
echo "  5. Then run: git push -u origin main"
echo ""
echo "To install the plugin locally:"
echo "  make install-user"
echo ""
echo "Plugin will be installed to:"
echo "  ~/.lv2/StudioReverb.lv2/"
echo "  ~/.vst/StudioReverb-vst.so"
echo "  ~/.vst3/StudioReverb.vst3/"