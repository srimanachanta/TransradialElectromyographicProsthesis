#include "ServoBonnet.h"
#include <cmath>

ServoBonnet::ServoBonnet(const char* bus_filename, int channel) {
  pca = std::make_shared<PCA9685>(bus_filename, channel);
  // All servos use a frequency of 50, PCA9685 doesn't support configuring an
  // individual channel's frequency.
  pca->SetPWMFrequency(50);

  // Create all servo objects at once, slight overhead but simpler than creating
  // at call time
  servos.reserve(16);
  for (int i = 0; i < 16; i++) {
    servos.emplace_back(
        std::make_shared<Servo>(std::make_unique<PWMChannel>(i, pca), M_PI));
  }
}

std::shared_ptr<Servo> ServoBonnet::GetServo(const uint8_t idx) {
  return servos.at(idx);
}
