#include "Servo.h"

#include <stdexcept>

Servo::Servo(std::unique_ptr<PWMChannel> channel, double mechanicalRange) {
  this->channel = std::move(channel);
  this->mechanicalRange = mechanicalRange;
}

void Servo::SetPulseWidthRange(const uint16_t min_pulse_microseconds,
                               const uint16_t max_pulse_microseconds) {
  minDuty =
      (min_pulse_microseconds * channel->GetFrequency()) / 1000000.0 * 0xFFFF;
  maxDuty =
      (max_pulse_microseconds * channel->GetFrequency()) / 1000000.0 * 0xFFFF;
  dutyRange = maxDuty - minDuty;
}

double Servo::GetAngle() const {
  auto const currentDutyCycle = channel->GetDutyCycle();
  return (currentDutyCycle - minDuty) / dutyRange * mechanicalRange;
}

void Servo::SetAngle(const double angle_rad) const {
  if (angle_rad < 0 || angle_rad > mechanicalRange) {
    throw std::runtime_error("Servo angle is out of range");
  }

  auto const frac = angle_rad / mechanicalRange;
  auto const dutyOut = static_cast<uint16_t>(minDuty + frac * dutyRange);

  channel->SetDutyCycle(dutyOut);
}
