# Build Instructions for Studio Reverb

## Prerequisites

### Required Dependencies
- C++11 compatible compiler (GCC 5+, Clang 3.4+, MSVC 2015+)
- Make
- Git
- pkg-config (Linux/macOS)

### DPF Framework
Studio Reverb uses DPF (DISTRHO Plugin Framework) as a git submodule.

```bash
# Clone with submodules
git clone --recursive https://github.com/luna-co-software/StudioReverb.git
cd StudioReverb

# Or if already cloned, initialize submodules
git submodule update --init --recursive
```

## Building

### Quick Build (All Formats)
```bash
make
```

### Build Specific Formats
```bash
make lv2       # LV2 only
make vst2      # VST2 only
make vst3      # VST3 only
```

### Debug Build
```bash
make DEBUG=true
```

### Clean Build
```bash
make clean
make
```

### Parallel Build
```bash
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS
```

## Installation

### System-wide Installation
```bash
sudo make install
```

### User Installation (Recommended)
```bash
make install-user
```

This installs plugins to:
- LV2: `~/.lv2/StudioReverb.lv2/`
- VST2: `~/.vst/StudioReverb-vst.so`
- VST3: `~/.vst3/StudioReverb.vst3/`

### Custom Installation Path
```bash
make install PREFIX=/custom/path
```

## Platform-Specific Notes

### Linux
- Ensure you have development headers for your audio system
- For GUI support: `libgl1-mesa-dev` or equivalent
- Optional: `libcairo2-dev` for alternative rendering

### macOS
- Xcode Command Line Tools required
- Universal Binary: `make MACOS_UNIVERSAL=true`

### Windows (MSYS2)
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make
make WIN32=true  # 32-bit
make WIN64=true  # 64-bit
```

## Troubleshooting

### Missing DPF
If you get errors about missing DPF:
```bash
git submodule update --init --recursive
```

### Missing Freeverb3 Files
The Freeverb3 library files should be in `common/freeverb/`. If missing:
```bash
chmod +x copy_freeverb.sh
./copy_freeverb.sh
```

### Build Errors with OpenGL
If you encounter OpenGL-related errors:
```bash
make HAVE_OPENGL=false
```

### Link Errors
If you get undefined references during linking:
```bash
make clean
make SKIP_STRIPPING=true DEBUG=true
```

## Testing

After building, test the plugin with:

### Command Line (using JACK)
```bash
# Start JACK server
jackd -d alsa -r 48000 -p 256 -n 2

# Run standalone version (if built)
./bin/StudioReverb
```

### In a DAW
1. Rescan plugins in your DAW
2. Look for "Studio Reverb" under:
   - Manufacturer: "Luna Co. Audio"
   - Category: "Reverb" or "Effects"

### Verify Installation
```bash
# Check LV2
lv2ls | grep StudioReverb

# Check VST3
ls ~/.vst3/ | grep StudioReverb
```

## Development

### Enable Debug Output
```bash
make DEBUG=true SKIP_STRIPPING=true
```

### Profile Build
```bash
make NOOPT=false DEBUG=false
```

### Static Analysis
```bash
make clean
scan-build make
```

## Common Issues

### "No rule to make target"
- Ensure all submodules are initialized
- Check that Makefile paths are correct

### Audio Glitches
- Increase buffer size in your DAW
- Disable power management features
- Use real-time kernel (Linux)

### UI Not Showing
- Verify OpenGL drivers are installed
- Try software rendering: `LIBGL_ALWAYS_SOFTWARE=1`

## Support

For issues and questions:
- GitHub Issues: https://github.com/luna-co-software/StudioReverb/issues
- Documentation: See README.md

## License

GPL-3.0 - See LICENSE file for details