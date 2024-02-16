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

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

void fail(beast::error_code ec, char const* what) {
    if( ec == net::error::operation_aborted ||
        ec == websocket::error::closed)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}

// Echoes back all received WebSocket messages
class websocket_session : public std::enable_shared_from_this<websocket_session> {
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
    std::function<std::optional<beast::detail::ConstBufferSequence>()> data_supplier;

    void on_run() {
        // Set suggested timeout settings for the websocket
        ws_.set_option(
                websocket::stream_base::timeout::suggested(
                        beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        ws_.set_option(websocket::stream_base::decorator(
                [](websocket::response_type& res)
                {
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
        if(ec)
            return fail(ec, "accept");

        ws_.async_write(
                buffer_.data(),
                beast::bind_front_handler(
                        &websocket_session::on_write,
                        shared_from_this()));
    }

    void on_write(beast::error_code ec, std::size_t) {
        if(ec)
            return fail(ec, "write");

        auto data = data_supplier();
        if(data == std::nullopt) {
            on_write(ec, 0);
        } else {
            ws_.async_write(
                    data.value(),
                    beast::bind_front_handler(
                            &websocket_session::on_write,
                            shared_from_this()));
        }
    }

public:
    // Take ownership of the socket
    explicit websocket_session(tcp::socket&& socket, std::function<std::optional<beast::detail::ConstBufferSequence>()> supplier) : ws_(std::move(socket)), data_supplier(std::move(supplier)) {
        std::cout << "Websocket Connection Opened" << std::endl;
    }

    ~websocket_session() {
        std::cout << "Websocket Connection Closed" << std::endl;
    }

    void run() {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.
        net::dispatch(ws_.get_executor(),
                      beast::bind_front_handler(
                              &websocket_session::on_run,
                              shared_from_this()));
    }
};