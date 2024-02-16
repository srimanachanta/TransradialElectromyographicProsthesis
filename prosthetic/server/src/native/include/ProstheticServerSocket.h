#pragma once

#include <boost/asio.hpp>
#include "websocket_listener.h"
#include <boost/smart_ptr.hpp>

class ProstheticServerSocket {
public:
    explicit ProstheticServerSocket(std::string_view address, unsigned short port) {
        const auto endpoint = boost::asio::ip::tcp::endpoint{boost::asio::ip::make_address(address), port};

        net::io_context ioc{1};
        std::make_shared<websocket_listener>(ioc, endpoint)->run();

//        std::thread th([&ioc]{ ioc.run(); });
        ioc.run();
    }
};
