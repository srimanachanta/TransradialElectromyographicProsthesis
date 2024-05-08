#ifndef PROSTHETICSERVODRIVER_H
#define PROSTHETICSERVODRIVER_H

#if defined(ENABLE_PROSTHETIC)
#include <PCA9685.h>
#else
#include <utility>

class PCA9685 {
 public:
  explicit PCA9685(const char* bus_filename, int address){};
  void Reset() const {};
  void Sleep() const {};
  void Wakeup() const {};
  void SetPWMFrequency(float frequency_hz) const {};
  [[nodiscard]]
  uint16_t GetPWMFrequency() const {
    return 0;
  };
  [[nodiscard]]
  std::pair<uint16_t, uint16_t> GetPWM(uint8_t channel) const {
    return std::make_pair(0, 0);
  };
  void SetPWM(uint8_t channel, uint16_t on_time, uint16_t off_time) const {};
  void SetPWMAll(uint16_t on_time, uint16_t off_time) const {};
};

#endif

#endif  // PROSTHETICSERVODRIVER_H
