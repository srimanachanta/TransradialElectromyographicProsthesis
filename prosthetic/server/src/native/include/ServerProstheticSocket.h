#pragma once

#include <boost/smart_ptr.hpp>
#include "websocket_listener.h"

struct ProstheticServoPositions {
    unsigned short thumbLeft;
    unsigned short thumbRight;
    unsigned short indexLeft;
    unsigned short indexRight;
    unsigned short middleLeft;
    unsigned short middleRight;
    unsigned short ringLeft;
    unsigned short ringRight;
    unsigned short pinkieLeft;
    unsigned short pinkieRight;
    unsigned short wristExtension;
    unsigned short wristFlexion;
};

struct ProstheticForceReadings {
    double thumbForce;
    double indexForce;
    double middleForce;
    double ringForce;
    double pinkieForce;
    double palmForce;
};

class ServerProstheticSocket {
public:
    explicit ServerProstheticSocket(std::string_view address, unsigned short port) {
        const auto endpoint = tcp::endpoint{net::ip::make_address(address), port};

        net::io_context ioc{1};
        std::make_shared<websocket_listener>(ioc, endpoint, []{return net::buffer("hello!");})->run();

        ioc.run();
    }

    void update_servo_positions(ProstheticServoPositions positions) {
//        size_t buf_size = sizeof(ProstheticServoPositions);
//        std::vector<char> buffer(buf_size);
//        std::memcpy(buffer.data(), &positions, buf_size);
    }
};
