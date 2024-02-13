#pragma once

#include <boost/asio.hpp>
#include <iostream>

class EMGSensorArray {
public:
    explicit EMGSensorArray(const std::string &port)
            : io(), serial(io, port), enabled(false) {
        serial.set_option(boost::asio::serial_port_base::baud_rate(115200));
    }

    ~EMGSensorArray() {
        serial.close();
    }

    void Enable() {
        if(!enabled) {
            boost::asio::write(serial, boost::asio::buffer(&START_MESSAGE, 1));
            enabled = true;
        }
    }

    void Disable() {
        if(enabled) {
            boost::asio::write(serial, boost::asio::buffer(&STOP_MESSAGE, 1));
            enabled = false;
        }
    }

    void SyncClock() {
        boost::asio::write(serial, boost::asio::buffer(&SYNC_ENABLE_TIME_MESSAGE, 1));
    }

    std::pair<long, std::vector<double>> GetDataFrame() {
        auto data = ReadLine();
        std::stringstream data_stream(data);

        long timestamp;
        std::vector<double> adc_data;
        for (int i = 0; i < 7; i++) {
            long v;
            data_stream >> v;

            if (i == 0) {
                timestamp = v;
            } else {
                adc_data.push_back((double) v * (5.0 / 4095.0));
            }

            if (data_stream.peek() == ',')
                data_stream.ignore();
        }

        return std::make_pair(timestamp, adc_data);
    }

private:
    static constexpr int START_MESSAGE = 0x1;
    static constexpr int STOP_MESSAGE = 0x2;
    static constexpr int SYNC_ENABLE_TIME_MESSAGE = 0x3;

    boost::asio::io_service io;
    boost::asio::serial_port serial;
    bool enabled;

    std::string ReadLine() {
        std::string result;
        char c;
        while (true) {
            boost::asio::read(serial, boost::asio::buffer(&c, 1));
            switch (c) {
                case '\r':
                    break;
                case '\n':
                    return result;
                default:
                    result += c;
            }
        }
    }
};
