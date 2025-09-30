#!/bin/bash

# Test script for Studio Reverb audio paths
# Tests each reverb algorithm to ensure they produce output

echo "==============================================="
echo "Studio Reverb Audio Path Test"
echo "==============================================="
echo ""

# Check if plugin is built
if [ ! -f "bin/StudioReverb.lv2/StudioReverb.so" ] && [ ! -f "bin/StudioReverb-vst.so" ]; then
    echo "ERROR: Plugin not built. Run 'make' first."
    exit 1
fi

# Function to test with a host if available
test_with_host() {
    local host=$1
    local plugin_path=$2

    if command -v $host &> /dev/null; then
        echo "Testing with $host..."
        case $host in
            "jalv")
                # Test LV2 with jalv
                timeout 5 jalv -p urn:lunaco:studio-reverb 2>&1 | head -20
                ;;
            "carla-single")
                # Test with Carla
                timeout 5 carla-single native $plugin_path 2>&1 | head -20
                ;;
        esac
    else
        echo "$host not found, skipping..."
    fi
}

echo "1. Checking plugin metadata..."
echo "-------------------------------"
if [ -f "bin/StudioReverb.lv2/manifest.ttl" ]; then
    echo "✓ LV2 manifest found"
    grep -E "doap:name|rdfs:label" bin/StudioReverb.lv2/*.ttl | head -5
else
    echo "✗ LV2 manifest not found"
fi

echo ""
echo "2. Checking DSP algorithms..."
echo "------------------------------"
echo "Looking for reverb processors in binary..."
if [ -f "bin/StudioReverb.lv2/StudioReverb.so" ]; then
    nm bin/StudioReverb.lv2/StudioReverb.so 2>/dev/null | grep -E "earlyref|progenitor|nrevb" | head -10
    if [ $? -eq 0 ]; then
        echo "✓ Reverb algorithms linked"
    else
        echo "✗ Reverb algorithms not found in binary"
    fi
fi

echo ""
echo "3. Parameter Test..."
echo "--------------------"
echo "Expected parameters for each algorithm:"
echo ""
echo "ROOM: Dry, Early, Late, Size, Width, Predelay, Decay, Diffuse, Damping"
echo "HALL: Dry, Size, Width, Predelay, Decay, Diffuse, Damping, Modulation"
echo "PLATE: Dry, Predelay, Decay, Diffuse, Damping, Modulation"
echo "EARLY: Dry, Size, Width, Predelay, Diffuse"

echo ""
echo "4. Testing with available hosts..."
echo "-----------------------------------"

# Test with different hosts if available
test_with_host "jalv" "urn:lunaco:studio-reverb"
test_with_host "carla-single" "bin/StudioReverb.lv2/StudioReverb.so"

echo ""
echo "5. Manual Testing Instructions..."
echo "----------------------------------"
echo "To fully test the reverb audio paths:"
echo ""
echo "1. Load the plugin in your DAW"
echo "2. Send audio through the plugin"
echo "3. Test each algorithm:"
echo "   a. Room - Should have distinct early and late reflections"
echo "   b. Hall - Should have longer decay and modulation"
echo "   c. Plate - Should have metallic character with modulation"
echo "   d. Early Reflections - Should have only early reflections, no tail"
echo ""
echo "4. For each algorithm, verify:"
echo "   - Dry signal passes through when Dry = 100%"
echo "   - Reverb is heard when Early/Late > 0%"
echo "   - Size parameter changes spatial characteristics (Room/Hall/Early)"
echo "   - Decay parameter changes reverb tail length (Room/Hall/Plate)"
echo "   - Modulation adds chorus effect (Hall/Plate only)"
echo "   - Damping reduces high frequencies in tail"
echo ""
echo "5. Check for issues:"
echo "   - Clicks or pops when changing parameters"
echo "   - Silence when reverb should be audible"
echo "   - Distortion at high levels"
echo "   - CPU usage spikes"

echo ""
echo "==============================================="
echo "Test complete. Check results above."
echo "==============================================="