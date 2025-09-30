#!/bin/bash

# Cleanup script to reorganize the repository structure
# Moves plugin files to root and removes unnecessary directories

echo "Reorganizing StudioReverb repository structure..."

# Check if we're in the right directory
if [ ! -f "README.md" ] || [ ! -f "Makefile" ]; then
    echo "Error: Please run this script from the StudioReverb root directory"
    exit 1
fi

# Copy source files from plugins subdirectory if they exist
if [ -d "plugins/studio-reverb" ]; then
    echo "Moving source files from plugins/studio-reverb to root..."

    # Copy DSP.cpp if it doesn't exist in root
    if [ ! -f "DSP.cpp" ] && [ -f "plugins/studio-reverb/DSP.cpp" ]; then
        cp "plugins/studio-reverb/DSP.cpp" .
        echo "  ✓ Copied DSP.cpp"
    fi

    # Copy UI.cpp if it doesn't exist in root
    if [ ! -f "UI.cpp" ] && [ -f "plugins/studio-reverb/UI.cpp" ]; then
        cp "plugins/studio-reverb/UI.cpp" .
        echo "  ✓ Copied UI.cpp"
    fi

    # Remove the plugins directory
    echo "Removing plugins directory..."
    rm -rf plugins/
    echo "  ✓ Removed plugins directory"
fi

# Update copy_freeverb.sh to use the correct paths
if [ -f "copy_freeverb.sh" ]; then
    echo "Updating copy_freeverb.sh paths..."
    # Already correct, no changes needed
fi

# Verify the structure
echo ""
echo "Verifying new structure..."
echo "Main source files:"
for file in DistrhoPluginInfo.h Plugin.cpp DSP.cpp DSP.hpp UI.cpp UI.hpp; do
    if [ -f "$file" ]; then
        echo "  ✓ $file"
    else
        echo "  ✗ $file (missing - needs to be created)"
    fi
done

echo ""
echo "Build files:"
for file in Makefile README.md BUILD.md; do
    if [ -f "$file" ]; then
        echo "  ✓ $file"
    else
        echo "  ✗ $file (missing)"
    fi
done

echo ""
echo "Reorganization complete!"
echo ""
echo "New structure:"
echo "  StudioReverb/"
echo "  ├── DistrhoPluginInfo.h  (plugin configuration)"
echo "  ├── Plugin.cpp            (main plugin implementation)"
echo "  ├── DSP.cpp              (DSP implementation)"
echo "  ├── DSP.hpp              (DSP header)"
echo "  ├── UI.cpp               (user interface)"
echo "  ├── UI.hpp               (UI header)"
echo "  ├── Makefile             (build configuration)"
echo "  ├── README.md            (project documentation)"
echo "  ├── BUILD.md             (build instructions)"
echo "  ├── common/              (Freeverb3 library)"
echo "  │   └── freeverb/"
echo "  ├── dpf/                 (DPF framework - git submodule)"
echo "  └── bin/                 (built binaries - created after build)"
echo ""
echo "Next steps:"
echo "  1. If not done: git submodule add https://github.com/DISTRHO/DPF.git dpf"
echo "  2. Run: ./copy_freeverb.sh"
echo "  3. Build: make -j4"