# Studio Reverb

A high-quality reverb plugin combining four distinct algorithms from the Dragonfly Reverb suite into a single, unified plugin.

## Overview

Studio Reverb is an open-source audio plugin that provides professional-grade reverb processing with four selectable algorithms:
- **Room**: Small to medium room simulation
- **Hall**: Large concert hall simulation
- **Plate**: Vintage plate reverb emulation
- **Early Reflections**: Isolated early reflections without late reverb

## Features

- Four high-quality reverb algorithms in one plugin
- Dynamic UI that adapts to show relevant controls for each algorithm
- Based on the acclaimed Freeverb3 DSP library
- Available in LV2, VST2, and VST3 formats
- Cross-platform support (Linux, Windows, macOS)

## Building

### Requirements
- DPF (DISTRHO Plugin Framework)
- C++11 compatible compiler
- Make

### Build Instructions

```bash
# Clone the repository with submodules
git clone --recursive https://github.com/luna-co-software/StudioReverb.git
cd StudioReverb

# Build all plugin formats
make

# Or build specific formats
make lv2
make vst2
make vst3
```

### Installation

After building, the plugins will be in the `bin` directory. Copy them to your plugin directories:

- **LV2**: `~/.lv2/` or `/usr/lib/lv2/`
- **VST2**: `~/.vst/` or `/usr/lib/vst/`
- **VST3**: `~/.vst3/` or `/usr/lib/vst3/`

## Parameters

### Common Parameters (All Algorithms)
- **Dry Level**: Direct input signal level (0-100%)
- **Early Level**: Early reflections level (0-100%)
- **Late Level**: Late reverberation level (0-100%)
- **Width**: Stereo width (0-100%)
- **Pre-Delay**: Reverb onset delay (0-200ms)
- **Low Cut**: High-pass filter (20-500Hz)
- **High Cut**: Low-pass filter (1-20kHz)

### Algorithm-Specific Parameters

#### Room & Hall
- **Size**: Room/Hall dimensions (0-100%)
- **Decay**: Reverb tail length (0.1-10s)
- **Diffusion**: Echo density (0-100%)
- **Damping**: High-frequency absorption (0-100%)

#### Hall & Plate Only
- **Modulation**: Chorus effect on late reverb (0-100%)

#### Early Reflections Only
- **Size**: Reflection pattern size
- **Diffusion**: Reflection complexity

## License

This project is licensed under the GPL-3.0 License - see the LICENSE file for details.

## Credits

Based on Dragonfly Reverb by Michael Willis and Rob van den Berg.
Uses Freeverb3 DSP library by Teru Kamogashira.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.