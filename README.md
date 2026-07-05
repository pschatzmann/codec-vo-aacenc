# VOAACEncoder

[![Arduino Library](https://img.shields.io/badge/Arduino-Library-blue.svg)](https://www.arduino.cc/reference/en/libraries/)
[![CMake](https://img.shields.io/badge/CMake-Ready-blue.svg)](#cmake-usage)
[![ESP-IDF Component](https://img.shields.io/badge/ESP--IDF-Component-blue.svg)](#esp-idf-component-usage)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](COPYING)


This folder contains an Arduino-compatible packaging of the VisualOn `vo-aacenc` encoder. 

Compared with Fraunhofer FDK AAC, vo-aacenc is simpler and is lightweight enough for older or resource-constrained systems. However, it offers lower audio quality, supports only AAC-LC, lacks advanced features such as high-quality VBR and HE-AAC profiles, and is no longer actively maintained.

Despite these limitations, its low complexity and modest resource requirements make it well suited for many microcontroller applications.


## What is included

- Original C encoder implementation sources copied into `src/vo-aacenc`
- Original encoder headers copied into `src/vo-aacenc`
- Simple C++ wrapper class: `VOAACEncoder`
- Example sketch: `examples/EncodeSineToAAC`
- CMake compliant

## Basic usage

```cpp
#include <VOAACEncoder.h>

VOAACEncoder encoder;

void setup() {
  // encoder.begin(sampleRateHz, bitRateBps, channels, useAdts)
  encoder.begin(16000, 32000, 1, true);
}

void loop() {
  int16_t pcm[VOAACEncoder::kFrameSamplesPerChannel];
  uint8_t out[2048];
  size_t outBytes = 0;
  encoder.encodeFrame(pcm, VOAACEncoder::kFrameSamplesPerChannel, out, sizeof(out), outBytes);
}
```

## Notes

Parameters for begin():

- sampleRateHz: Input sample rate in Hz (must be one of the supported rates listed below).
- bitRateBps: Target AAC bitrate in bits per second (e.g. 32000 = 32 kbps).
- channels: Number of input channels (1 = mono, 2 = stereo).
- useAdts: When true, ADTS headers are prepended to each encoded frame (useful for streaming/playback tools).


- Input frame size must be exactly 1024 samples per channel.
- Input PCM format is 16-bit signed, interleaved if stereo.
- Typical AAC output fits into 2 KB for one frame at common speech bitrates, but size depends on settings.
- Uses PSRAM for ESP32 or Raspberry Pi Pico when available and enabled.
- Resource guidance: This encoder performs best on 32-bit MCUs with at least several hundred kilobytes of RAM (for example ESP32, RP2040 with PSRAM, or similar). It is generally unsuitable for 8-bit AVR boards (Uno/Nano) or very small Cortex-M0 devices without external memory. If you're targeting constrained hardware, prefer lower sample rates, mono audio, enable PSRAM where supported, or consider a simpler codec.
- Bitrate note: `bitRate` is specified in bits per second. The encoder expects per-channel bitrates in the practical range 4000–160000 bps (4 kbps–160 kbps). The implementation also enforces an overall cap roughly equal to sampleRate * 6 * channels. Internally the encoder selects one of the discrete bandwidth presets (per-channel): 16, 24, 32, 40, 48, 56, 64 kbps — your requested bitrate will be mapped to the closest preset for bandwidth selection. Example: for mono 32 kbps pass `bitRate = 32000`.
- Supported sample rates: 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 64000, 88200, 96000 Hz.

## Recommendation

Though it is possible to use this library directly, I recommend to use it as part of the [AudioTools Library](https://github.com/pschatzmann/arduino-audio-tools) which provides a standardized API for all encoders and decoders. For further details see the [corresponding Wiki](https://github.com/pschatzmann/arduino-audio-tools/wiki/Encoding-and-Decoding-of-Audio).


## Install in Arduino

1. In Arduino IDE, add this folder as a ZIP library or copy `VOAACEncoder` into your `libraries` directory.
2. Restart Arduino IDE and open the example sketch.

## CMake usage

Build this library directly:

```sh
cmake -S . -B build
cmake --build build -j
```

Use it from another CMake project:

```cmake
add_subdirectory(path/to/VOAACEncoder)
target_link_libraries(your_target PRIVATE VOAACEncoder)
```

Headers are exposed from `src/`, so you can include:

```cpp
#include <VOAACEncoder.h>
```

## ESP-IDF component usage

This repository can be used directly as an ESP-IDF component.

In your ESP-IDF project `CMakeLists.txt`:

```cmake
idf_component_register(...)
```

Then add this repository under your project's `components/` folder (for example `components/codec-vo-aacenc`).

