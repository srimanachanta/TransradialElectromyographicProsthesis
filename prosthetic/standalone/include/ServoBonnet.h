#pragma once

#include <vector>
#include "ProstheticServoDriver.h"
#include "Servo.h"

class ServoBonnet {
 public:
  explicit ServoBonnet(const char* bus_filename, int channel = 0x40);

  std::shared_ptr<Servo> GetServo(uint8_t idx);

 private:
  std::shared_ptr<PCA9685> pca;
  std::vector<std::shared_ptr<Servo>> servos;
};
