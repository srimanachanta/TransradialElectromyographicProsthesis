#pragma once

#include <cstdint>
#include <PCA9685.h>
#include <memory>

class PWMChannel {
public:
    explicit PWMChannel(uint8_t channel, std::shared_ptr<PCA9685> pca);

    uint16_t GetFrequency();

    uint16_t GetDutyCycle();

    void SetDutyCycle(uint16_t duty_cycle);

private:
    uint8_t channel;
    std::shared_ptr<PCA9685> pca;
};
