#ifndef VOAAC_ENCODER_ARDUINO_H
#define VOAAC_ENCODER_ARDUINO_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include <string.h>

extern "C" {
#include "vo-aacenc/cmnMemory.h"
#include "vo-aacenc/voAAC.h"
}

/**
 * @brief Lightweight C++ wrapper around the VisualOn AAC encoder API.
 *
 * This class owns the encoder handle and exposes a simple frame-based API for
 * Arduino and non-Arduino builds. The encoder expects exactly 1024 PCM samples
 * per channel for each call to encodeFrame().
 */
class VOAACEncoder {
public:
  /** Number of PCM samples required per channel for one AAC frame. */
  static const size_t kFrameSamplesPerChannel = 1024;

  /** Construct an encoder wrapper in the stopped state. */
  VOAACEncoder()
      : api_(), handle_(0), memOp_(), channels_(0), started_(false) {}

  /**
   * Destroy the wrapper and release any encoder resources.
   */
  ~VOAACEncoder() { end(); }

  /**
   * @brief Initialize the encoder.
   *
   * @param sampleRate Input sample rate in Hz.
   * @param bitRate Target AAC bitrate in bps.
   * @param channels Number of input channels (1 or 2).
   * @param useAdts True to prepend ADTS headers to each output frame.
   * @return True on success, false on invalid parameters or encoder init error.
   */
  bool begin(uint32_t sampleRate, uint32_t bitRate, uint8_t channels,
             bool useAdts = true) {
    end();

    if (channels < 1 || channels > 2) {
      return false;
    }

    if (!isSampleRateSupported(sampleRate)) {
      return false;
    }

    if (voGetAACEncAPI(&api_) != VO_ERR_NONE) {
      return false;
    }

    memOp_.Alloc = cmnMemAlloc;
    memOp_.Copy = cmnMemCopy;
    memOp_.Free = cmnMemFree;
    memOp_.Set = cmnMemSet;
    memOp_.Check = cmnMemCheck;

    VO_CODEC_INIT_USERDATA userData;
    memset(&userData, 0, sizeof(userData));
    userData.memflag = VO_IMF_USERMEMOPERATOR;
    userData.memData = &memOp_;

    if (api_.Init(&handle_, VO_AUDIO_CodingAAC, &userData) != VO_ERR_NONE) {
      handle_ = 0;
      return false;
    }

    AACENC_PARAM params;
    memset(&params, 0, sizeof(params));
    params.sampleRate = static_cast<int>(sampleRate);
    params.bitRate = static_cast<int>(bitRate);
    params.nChannels = static_cast<short>(channels);
    params.adtsUsed = useAdts ? 1 : 0;

    if (api_.SetParam(handle_, VO_PID_AAC_ENCPARAM, &params) != VO_ERR_NONE) {
      end();
      return false;
    }

    channels_ = channels;
    started_ = true;
    return true;
  }

  /**
   * @brief Encode one AAC frame from interleaved 16-bit PCM input.
   *
   * @param pcmInterleaved Pointer to interleaved PCM input.
   * @param samplesPerChannel Must be kFrameSamplesPerChannel (1024).
   * @param output Destination buffer for encoded AAC bytes.
   * @param outputCapacity Size of output buffer in bytes.
   * @param outputBytes Receives the number of bytes written to output.
   * @return True when a frame was encoded, false on invalid state/arguments or
   *         when the underlying encoder reports an error.
   */
  bool encodeFrame(const int16_t *pcmInterleaved, size_t samplesPerChannel,
                   uint8_t *output, size_t outputCapacity,
                   size_t &outputBytes) {
    outputBytes = 0;

    if (!started_ || pcmInterleaved == NULL || output == NULL) {
      return false;
    }

    if (samplesPerChannel != kFrameSamplesPerChannel) {
      return false;
    }

    VO_CODECBUFFER input;
    memset(&input, 0, sizeof(input));
    input.Buffer = (uint8_t *)(const_cast<int16_t *>(pcmInterleaved));
    input.Length =
        static_cast<VO_U32>(samplesPerChannel * channels_ * sizeof(int16_t));

    VO_CODECBUFFER out;
    memset(&out, 0, sizeof(out));
    out.Buffer = output;
    out.Length = static_cast<VO_U32>(outputCapacity);

    VO_AUDIO_OUTPUTINFO outInfo;
    memset(&outInfo, 0, sizeof(outInfo));

    if (api_.SetInputData(handle_, &input) != VO_ERR_NONE) {
      return false;
    }

    if (api_.GetOutputData(handle_, &out, &outInfo) != VO_ERR_NONE) {
      return false;
    }

    outputBytes = out.Length;
    return true;
  }

  /**
   * @brief Stop the encoder and release all allocated resources.
   */
  void end() {
    if (started_ && handle_ != 0) {
      api_.Uninit(handle_);
    }
    handle_ = 0;
    channels_ = 0;
    started_ = false;
    memset(&api_, 0, sizeof(api_));
  }

  /**
   * @brief Check whether the encoder is initialized and ready.
   *
   * @return True if begin() succeeded and end() was not called.
   */
  bool isReady() const { return started_; }

  static constexpr int kSupportedSampleRatesCount = 12;
  static constexpr int kSupportedSampleRates[kSupportedSampleRatesCount] = {
      96000, 88200, 64000, 48000, 44100, 32000,
      24000, 22050, 16000, 12000, 11025, 8000};

public:
  /** Check whether a sample rate (Hz) is supported by the encoder. */
  static bool isSampleRateSupported(uint32_t sampleRate) {
    for (int i = 0; i < kSupportedSampleRatesCount; ++i) {
      if (static_cast<uint32_t>(kSupportedSampleRates[i]) == sampleRate)
        return true;
    }
    return false;
  }

#ifdef ARDUINO
  /** Print supported sample rates to Serial (Arduino builds). */
  static void printSupportedSampleRates() {
    Serial.print("Supported sample rates (Hz): ");
    for (int i = 0; i < kSupportedSampleRatesCount; ++i) {
      if (i) Serial.print(", ");
      Serial.print(kSupportedSampleRates[i]);
    }
    Serial.println();
  }
#endif


private:
  VO_AUDIO_CODECAPI api_;
  VO_HANDLE handle_;
  VO_MEM_OPERATOR memOp_;
  uint8_t channels_;
  bool started_;
};

#endif
