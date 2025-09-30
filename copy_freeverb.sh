#!/bin/bash

# Copy Freeverb3 library files from the original location
# This script sets up the common/freeverb directory with necessary files

SOURCE_DIR="/home/marc/projects/plugins/plugins/StudioReverbDPF/common/freeverb"
DEST_DIR="common/freeverb"

echo "Setting up Freeverb3 library files..."

# Create destination directory
mkdir -p $DEST_DIR

# Copy essential files for our reverb algorithms
echo "Copying core files..."
cp -v $SOURCE_DIR/*.hpp $DEST_DIR/ 2>/dev/null || echo "Headers already exist or not found"
cp -v $SOURCE_DIR/*.cpp $DEST_DIR/ 2>/dev/null || echo "Sources already exist or not found"
cp -v $SOURCE_DIR/*.h $DEST_DIR/ 2>/dev/null || echo "C headers already exist or not found"

# Copy license and documentation
cp -v $SOURCE_DIR/COPYING $DEST_DIR/ 2>/dev/null
cp -v $SOURCE_DIR/AUTHORS $DEST_DIR/ 2>/dev/null
cp -v $SOURCE_DIR/README $DEST_DIR/ 2>/dev/null

echo "Freeverb3 library setup complete!"
echo ""
echo "Files needed for Studio Reverb:"
echo "  - earlyref.* (Early reflections)"
echo "  - progenitor2.* (Room/Hall late reverb)"
echo "  - nrevb.* (Plate reverb)"
echo "  - Supporting files (delay, comb, allpass, biquad, etc.)"

# Check if critical files exist
if [ -f "$DEST_DIR/earlyref.cpp" ] && [ -f "$DEST_DIR/progenitor2.cpp" ]; then
    echo ""
    echo "✓ Critical files found. Ready to build!"
else
    echo ""
    echo "⚠ Some files may be missing. You may need to manually copy from:"
    echo "  $SOURCE_DIR"
fi