#pragma once

#include <cstdint>
#include "ProstheticServoDriver.h"
#include <memory>

class PWMChannel {
 public:
  explicit PWMChannel(uint8_t channel, std::shared_ptr<PCA9685> pca);

  [[nodiscard]] uint16_t GetFrequency() const;

  [[nodiscard]] uint16_t GetDutyCycle() const;

  void SetDutyCycle(uint16_t duty_cycle) const;

 private:
  uint8_t channel;
  std::shared_ptr<PCA9685> pca;
};
