#pragma once

#include "I2CBus.h"
#include <memory>

class PCA9685 {
public:
    explicit PCA9685(const char *bus_filename, int address);

    /**
     * Sends reset command to the IC
     */
    void Reset();

    /**
     * Puts the IC into sleep mode. (Sets SLEEP bit high).
     */
    void Sleep();

    /**
     * Pulls the IC out of sleep mode. (Sets SLEEP bit low).
     */
    void Wakeup();

    /**
     * Sets the PWM frequency for the entire chip, up to ~1.6 KHz
     * @param frequency_hz target frequency, will be rounded to set, then set
     */
    void SetPWMFrequency(float frequency_hz);

    /**
     * Get the current set PWM frequency of the IC in Hz
     * @return current frequency in Hz
     */
    uint16_t GetPWMFrequency();

    /**
     * Get the ON and OFF times for a single LED channel of the IC
     * @param channel channel to get
     * @return pair of the ON and OFF ticks
     */
    std::pair<uint16_t, uint16_t> GetPWM(uint8_t channel);

    /**
     * Set the ON and OFF times for a single LED channel of the IC
     * @param channel channel to set
     * @param on_time Which tick to set ON 0 to 4095 inclusive
     * @param off_time Which tick to set OFF 0 to 4095 inclusive
     */
    void SetPWM(uint8_t channel, uint16_t on_time, uint16_t off_time);

    /**
     * Set the ON and OFF times for all LED channels of the IC
     * @param on_time Which tick to set ON 0 to 4095 inclusive
     * @param off_time Which tick to set OFF 0 to 4095 inclusive
     */
    void SetPWMAll(uint16_t on_time, uint16_t off_time);

private:
    // Values for Datasheet: https://cdn-shop.adafruit.com/datasheets/PCA9685.pdf
    // Registers
    static constexpr uint8_t MODE1 = 0x00;
    static constexpr uint8_t MODE2 = 0x01;
    static constexpr uint8_t SUBADR1 = 0x02;
    static constexpr uint8_t SUBADR2 = 0x03;
    static constexpr uint8_t SUBADR3 = 0x04;
    static constexpr uint8_t PRESCALE = 0xFE;
    static constexpr uint8_t LED0_ON_L = 0x06;
    static constexpr uint8_t LED0_ON_H = 0x07;
    static constexpr uint8_t LED0_OFF_L = 0x08;
    static constexpr uint8_t LED0_OFF_H = 0x09;
    static constexpr uint8_t ALL_LED_ON_L = 0xFA;
    static constexpr uint8_t ALL_LED_ON_H = 0xFB;
    static constexpr uint8_t ALL_LED_OFF_L = 0xFC;
    static constexpr uint8_t ALL_LED_OFF_H = 0xFD;

    // Bits
    static constexpr uint8_t RESTART = 0x80;
    static constexpr uint8_t SLEEP = 0x10;
    static constexpr uint8_t AI = 0x20;
    static constexpr uint8_t ALLCALL = 0x01;
    static constexpr uint8_t INVRT = 0x10;
    static constexpr uint8_t OUTDRV = 0x04;

    // Frequency of the IC's internal oscillator
    static constexpr int OSCILLATOR_FREQUENCY_HZ = 2.5e7;
    static constexpr uint8_t MIN_PRESCALE = 0x03;
    static constexpr uint8_t MAX_PRESCALE = 0xFF;

    std::unique_ptr<I2CBus> bus;
};
