#include "I2CBus.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <system_error>

extern "C" {
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
}

I2CBus::I2CBus(const char* bus_filename, const int address) {
  bus_fd = open(bus_filename, O_RDWR);

  if (bus_fd < 0) {
    throw std::system_error(errno, std::system_category(),
                            "Unable to open I2C bus");
  }

  if (ioctl(bus_fd, I2C_SLAVE, address) < 0) {
    throw std::system_error(errno, std::system_category(),
                            "Unable to set bus address");
  }
}

uint8_t I2CBus::ReadByte(const int command) const {
  auto const res = i2c_smbus_read_byte_data(bus_fd, command);

  if (res < 0) {
    throw std::system_error(
        errno, std::system_category(),
        "I2C read transaction failed at register: " + std::to_string(command));
  }

  return res;
}

void I2CBus::WriteByte(const int command, const uint8_t data) const {
  if (i2c_smbus_write_byte_data(bus_fd, command, data) < 0) {
    throw std::system_error(
        errno, std::system_category(),
        "I2C write transaction failed at register: " + std::to_string(command) +
            " when writing " + std::to_string(data));
  }
}
