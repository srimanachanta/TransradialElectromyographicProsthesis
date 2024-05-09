#pragma once

#include "PWMChannel.h"
#include <memory>

class Servo {
 public:
  explicit Servo(std::unique_ptr<PWMChannel> channel, double mechanicalRange);

  void SetPulseWidthRange(uint16_t min_pulse_microseconds,
                          uint16_t max_pulse_microseconds);

  [[nodiscard]]
  double GetAngle() const;

  void SetAngle(double angle_rad) const;

 private:
  std::unique_ptr<PWMChannel> channel;

  double mechanicalRange;
  double minDuty{};
  double maxDuty{};
  double dutyRange{};
};
