#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "SystemDynamics.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

void fail(beast::error_code ec, char const *what) {
    if (ec == net::error::operation_aborted ||
        ec == websocket::error::closed)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}

// Echoes back all received WebSocket messages
class websocket_session : public std::enable_shared_from_this<websocket_session> {
private:
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;

    void on_run() {
        // Set suggested timeout settings for the websocket
        ws_.set_option(
                websocket::stream_base::timeout::suggested(
                        beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        ws_.set_option(websocket::stream_base::decorator(
                [](websocket::response_type &res) {
                    res.set(http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-server-async");
                }));
        // Accept the websocket handshake
        ws_.async_accept(
                beast::bind_front_handler(
                        &websocket_session::on_accept,
                        shared_from_this()));
    }

    void on_accept(beast::error_code ec) {
        if (ec)
            return fail(ec, "accept");

        std::cout << "Websocket Connection Opened" << std::endl;

        do_read();
    }

    void do_read() {
        ws_.async_read(
                buffer_,
                beast::bind_front_handler(
                        &websocket_session::on_read,
                        shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        // This indicates that the session was closed
        if (ec == websocket::error::closed)
            return;

        if (ec)
            return fail(ec, "read");

        auto currentServoPositions = net::buffer_cast<ProstheticServoPositions *>(buffer_.data());

        auto newPositions = ProstheticSystemDynamics::calculateServoPositions(*currentServoPositions);

        ws_.text(false);
        if(newPositions.has_value()) {
            ws_.async_write(
                    net::buffer(&newPositions.value(), sizeof(ProstheticServoPositions)),
                    beast::bind_front_handler(
                            &websocket_session::on_write,
                            shared_from_this()));
        } else {
            // There isn't a valid result, just stay as is until there is one
            ws_.async_write(
                    buffer_.data(),
                    beast::bind_front_handler(
                            &websocket_session::on_write,
                            shared_from_this()));
        }
    }

    void on_write(beast::error_code ec,
                  std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        // Clear the buffer
        buffer_.consume(buffer_.size());

        // Do another read
        do_read();
    }


public:
    explicit websocket_session(tcp::socket &&socket) : ws_(std::move(socket)) {}

    ~websocket_session() {
        std::cout << "Websocket Connection Closed" << std::endl;
    }

    void run() {
        net::dispatch(ws_.get_executor(),
                      beast::bind_front_handler(
                              &websocket_session::on_run,
                              shared_from_this()));
    }
};
