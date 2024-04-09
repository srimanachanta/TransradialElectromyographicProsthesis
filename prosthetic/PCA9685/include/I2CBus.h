#pragma once

#include <cinttypes>

/**
 * I2C Driver around the kernel's libi2c and SMBus.
 * https://www.kernel.org/doc/Documentation/i2c/dev-interface
 * https://github.com/barulicm/PiPCA9685/blob/master/src/I2CPeripheral.cpp
 */
class I2CBus {
public:
    explicit I2CBus(const char *bus_filename, int address);

    [[nodiscard]] uint8_t ReadByte(int command) const;

    void WriteByte(int command, uint8_t data) const;

private:
    int bus_fd;
};
