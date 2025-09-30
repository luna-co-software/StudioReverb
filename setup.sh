#!/bin/bash

# Setup script for StudioReverb repository

echo "Setting up StudioReverb repository..."

# Initialize git repository
echo "1. Initializing git repository..."
git init

# Add remote origin
echo "2. Adding remote origin..."
git remote add origin https://github.com/luna-co-software/StudioReverb.git

# Add DPF as submodule
echo "3. Adding DPF as git submodule..."
git submodule add https://github.com/DISTRHO/DPF.git dpf

# Initialize submodule
echo "4. Initializing DPF submodule..."
git submodule update --init --recursive

# Copy freeverb files if script exists
if [ -f "copy_freeverb.sh" ]; then
    echo "5. Copying Freeverb3 library files..."
    chmod +x copy_freeverb.sh
    ./copy_freeverb.sh
fi

# Create initial commit
echo "6. Creating initial commit..."
git add .
git commit -m "Initial commit: Studio Reverb - Unified Dragonfly Reverb Plugin

- Combines 4 reverb algorithms (Room, Hall, Plate, Early Reflections)
- Dynamic UI that adapts to show relevant controls per algorithm
- Based on Freeverb3 DSP library
- Built with DPF (DISTRHO Plugin Framework)
- Supports LV2, VST2, and VST3 formats"

echo ""
echo "Setup complete!"
echo ""
echo "Next steps:"
echo "  1. Build the plugin:  make -j4"
echo "  2. Test the plugin:   ./test_reverb.sh"
echo "  3. Push to GitHub:    git push -u origin main"
echo ""
echo "If you haven't created the GitHub repo yet:"
echo "  1. Go to https://github.com/new"
echo "  2. Create repository named 'StudioReverb'"
echo "  3. Make it public"
echo "  4. Don't initialize with README (we already have one)"
echo "  5. Then run: git push -u origin main"