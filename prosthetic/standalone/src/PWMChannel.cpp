#include "PWMChannel.h"

PWMChannel::PWMChannel(uint8_t channel, std::shared_ptr<PCA9685> pca) {
    this->channel = channel;
    this->pca = std::move(pca);
}

uint16_t PWMChannel::GetFrequency() {
    return pca->GetPWMFrequency();
}

uint16_t PWMChannel::GetDutyCycle() {
    auto const currentTimes = pca->GetPWM(channel);
    auto const onTime = currentTimes.first;
    auto const offTime = currentTimes.second;

    if (onTime == 0x1000 && offTime == 0) {
        return 0xFFFF; // Full duty cycle
    } else if (onTime == 0 && offTime == 0x1000) {
        return 0x0010; // Minimum duty cycle
    } else {
        // Reconstruct the duty cycle value (12-bit to 16-bit conversion)
        uint16_t duty_cycle = (offTime << 4);
        return duty_cycle;
    }
}

void PWMChannel::SetDutyCycle(uint16_t duty_cycle) {
    if(duty_cycle == 0xFFFF) {
        pca->SetPWM(channel, 0x1000, 0);
    } else if(duty_cycle < 0x0010) {
        pca->SetPWM(channel, 0, 0x1000);
    } else {
        // handle 16bit to 12bit conversion. There is resolution loss cause of this.
        uint16_t val = duty_cycle >> 4;
        pca->SetPWM(channel, 0, val);
    }
}
