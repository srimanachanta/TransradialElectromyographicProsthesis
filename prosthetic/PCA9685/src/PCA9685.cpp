#include "PCA9685.h"

#include <unistd.h>
#include <memory>

PCA9685::PCA9685(const char* bus_filename, int address) {
  bus = std::make_unique<I2CBus>(bus_filename, address);

  Reset();

  // Set a default PWM frequency that can be later overwritten
  SetPWMFrequency(1000);
}

void PCA9685::Reset() const {
  bus->WriteByte(MODE1, RESTART);
  // Restart must be high for at least 500μs
  usleep(5000);
}

void PCA9685::Sleep() const {
  uint8_t awake = bus->ReadByte(MODE1);
  uint8_t sleep = awake | SLEEP;  // set sleep BIT high
  bus->WriteByte(MODE1, sleep);
  usleep(5000);  // wait until cycle ends for sleep to be active
}

void PCA9685::Wakeup() const {
  uint8_t sleep = bus->ReadByte(MODE1);
  uint8_t wakeup = sleep & ~SLEEP;  // set sleep BIT low
  bus->WriteByte(MODE1, wakeup);
  usleep(5000);  // wait for the oscillator to wake up (500μs).
}

void PCA9685::SetPWMFrequency(const float frequency_hz) const {
  // Calculate prescale val. Datasheet 7.3.5 EQ2.
  auto prescale_val = (OSCILLATOR_FREQUENCY_HZ / (4096.0 * frequency_hz)) - 1.0;

  // clamp prescaler between min and max values from the datasheet
  if (prescale_val < MIN_PRESCALE) {
    prescale_val = MIN_PRESCALE;
  } else if (prescale_val > MAX_PRESCALE) {
    prescale_val = MAX_PRESCALE;
  }

  // Cast to byte
  auto const prescale = static_cast<uint8_t>(prescale_val);

  // cant set prescale unless sleep is high
  uint8_t oldMode = bus->ReadByte(MODE1);
  // set SLEEP to high
  uint8_t newMode = (oldMode & ~RESTART) | SLEEP;
  bus->WriteByte(MODE1, newMode);
  // once asleep we can set the prescaler value
  bus->WriteByte(PRESCALE, prescale);
  // return to the old mode
  bus->WriteByte(MODE1, oldMode);
  usleep(5000);  // allow the oscillator to wake up (500μs).
  // turn on auto increment. allows programming registers sequentially
  bus->WriteByte(MODE1, oldMode | RESTART | AI);
}

uint16_t PCA9685::GetPWMFrequency() const {
  uint8_t currentPrescale = bus->ReadByte(PRESCALE);
  return 2.5e7 / (4096.0 * (currentPrescale + 1));
}

std::pair<uint16_t, uint16_t> PCA9685::GetPWM(const uint8_t channel) const {
  // Determine registers to get using target channel and LED0 registers
  auto const channelOffset = 4 * channel;

  uint16_t onTick = bus->ReadByte(LED0_ON_L + channelOffset) |
                    (bus->ReadByte(LED0_ON_H + channelOffset) << 8);
  uint16_t offTick = bus->ReadByte(LED0_OFF_L + channelOffset) |
                     (bus->ReadByte(LED0_OFF_H + channelOffset) << 8);

  return std::make_pair(onTick, offTick);
}

void PCA9685::SetPWM(const uint8_t channel, const uint16_t on_time,
                     const uint16_t off_time) const {
  // Determine registers to set using target channel and LED0 registers
  auto const channelOffset = 4 * channel;
  bus->WriteByte(LED0_ON_L + channelOffset,
                 on_time & 0xFF);  // Shift to handle 16 to 12bit
  bus->WriteByte(LED0_ON_H + channelOffset, on_time >> 8);
  bus->WriteByte(LED0_OFF_L + channelOffset,
                 off_time & 0xFF);  // Shift to handle 16 to 12bit
  bus->WriteByte(LED0_OFF_H + channelOffset, off_time >> 8);
}

void PCA9685::SetPWMAll(const uint16_t on_time, const uint16_t off_time) const {
  bus->WriteByte(ALL_LED_ON_L, on_time & 0xFF);  // Shift to handle 16 to 12bit
  bus->WriteByte(ALL_LED_ON_H, on_time >> 8);
  bus->WriteByte(ALL_LED_OFF_L,
                 off_time & 0xFF);  // Shift to handle 16 to 12bit
  bus->WriteByte(ALL_LED_OFF_H, off_time >> 8);
}
