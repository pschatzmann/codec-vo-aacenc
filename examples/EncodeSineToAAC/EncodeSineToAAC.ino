#include <Arduino.h>
#include <math.h>
#include <VOAACEncoder.h>

VOAACEncoder encoder;

static int16_t pcmFrame[VOAACEncoder::kFrameSamplesPerChannel];
static uint8_t aacFrame[2048];

void fillSineFrame(float freqHz, uint32_t sampleRate) {
  static float phase = 0.0f;
  const float twoPi = 6.28318530718f;
  const float step = twoPi * freqHz / (float)sampleRate;

  for (size_t i = 0; i < VOAACEncoder::kFrameSamplesPerChannel; ++i) {
    pcmFrame[i] = (int16_t)(sin(phase) * 12000.0f);
    phase += step;
    if (phase >= twoPi) {
      phase -= twoPi;
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  if (!encoder.begin(16000, 32000, 1, true)) {
    Serial.println("AAC encoder init failed");
    return;
  }

  Serial.println("AAC encoder ready");
}

void loop() {
  if (!encoder.isReady()) {
    delay(1000);
    return;
  }

  fillSineFrame(440.0f, 16000);

  size_t outBytes = 0;
  bool ok = encoder.encodeFrame(
      pcmFrame,
      VOAACEncoder::kFrameSamplesPerChannel,
      aacFrame,
      sizeof(aacFrame),
      outBytes);

  if (!ok) {
    Serial.println("encodeFrame failed");
  } else {
    Serial.print("AAC bytes: ");
    Serial.println((unsigned long)outBytes);
  }

  delay(20);
}
