#pragma once

#include <vector>
#include <memory>
#include <PCA9685.h>
#include "Servo.h"

class ServoBonnet {
public:
    explicit ServoBonnet(const char *bus_filename = "/dev/i2c-1", int channel = 0x40);

    std::shared_ptr<Servo> GetServo(uint8_t idx);

private:
    std::shared_ptr<PCA9685> pca;
    std::vector<std::shared_ptr<Servo>> servos;
};
