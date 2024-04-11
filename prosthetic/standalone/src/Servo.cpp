#include "Servo.h"

#include <stdexcept>

Servo::Servo(std::unique_ptr<PWMChannel> channel, double mechanicalRange) {
    this->channel = std::move(channel);
    this->mechanicalRange = mechanicalRange;
}

void Servo::SetPulseWidthRange(uint16_t min_pulse_microseconds, uint16_t max_pulse_microseconds) {
    minDuty = (min_pulse_microseconds * channel->GetFrequency()) / 1000000.0 * 0xFFFF;
    maxDuty = (max_pulse_microseconds * channel->GetFrequency()) / 1000000.0 * 0xFFFF;
    dutyRange = maxDuty - minDuty;
}

double Servo::GetAngle() {
    auto const currentDutyCycle = channel->GetDutyCycle();
    return (currentDutyCycle - minDuty) / dutyRange * mechanicalRange;
}

void Servo::SetAngle(double angle_rad) {
    if(angle_rad < 0 || angle_rad > mechanicalRange) {
        throw std::runtime_error("Servo angle is out of range");
    }

    auto const frac = angle_rad / mechanicalRange;
    uint16_t dutyOut = minDuty + (uint16_t)(frac * dutyRange);

    channel->SetDutyCycle(dutyOut);
}
