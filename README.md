# VOAACEncoder (Arduino Port)

This folder contains an Arduino-compatible packaging of the VisualOn `vo-aacenc` encoder. 

Compared with the Fraunhofer FDK AAC encoder, `vo-aacenc` is using less resources at the cost of the audio quality.

## What is included

- Original C encoder implementation sources copied into `src/vo-aacenc`
- Original encoder headers copied into `src/vo-aacenc`
- Simple C++ wrapper class: `VOAACEncoder`
- Example sketch: `examples/EncodeSineToAAC`
- cmake complient

## Basic usage

```cpp
#include <VOAACEncoder.h>

VOAACEncoder encoder;

void setup() {
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

- Input frame size must be exactly 1024 samples per channel.
- Input PCM format is 16-bit signed, interleaved if stereo.
- Typical AAC output fits into 2 KB for one frame at common speech bitrates, but size depends on settings.
- Depending on MCU resources, this encoder may be too heavy for small boards.


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
