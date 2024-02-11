#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>



// T0, T1, F1-1, F1-2, F2-1, F2-2, F3-1, F3-2, F4-1, F4-2, F5-1, F5-2, F6-1, F6-2, W1, W2


class ClassifierSocket : public std::enable_shared_from_this<ClassifierSocket> {
    std::function<void(std::vector<float>)> dataConsumer;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
    boost::beast::flat_buffer buffer_;
    std::string host_;

    void on_resolve(boost::beast::error_code ec, const boost::asio::ip::tcp::resolver::results_type& results) {
        if(ec) { std::cerr << ec.message() << std::endl; }

        // Set the timeout for the operation
        boost::beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        boost::beast::get_lowest_layer(ws_).async_connect(
                results,
                boost::beast::bind_front_handler(
                        &ClassifierSocket::on_connect,
                        shared_from_this()));
    }

    void on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type ep) {
        if(ec) { std::cerr << ec.message() << std::endl; }

        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        boost::beast::get_lowest_layer(ws_).expires_never();

        // Set suggested timeout settings for the websocket
        ws_.set_option(
                boost::beast::websocket::stream_base::timeout::suggested(
                        boost::beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        ws_.set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::request_type& req)
                {
                    req.set(boost::beast::http::field::user_agent,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-client-async");
                }));

        // Update the host_ string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        host_ += ':' + std::to_string(ep.port());

        // Perform the websocket handshake
        ws_.async_handshake(host_, "/",
                            boost::beast::bind_front_handler(
                                    &ClassifierSocket::on_handshake,
                                    shared_from_this()));
    }

//    void on_handshake(boost::beast::error_code ec) {
//        if(ec) { std::cerr << ec.message() << std::endl; }
//
//    }
//
//    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred) {
//        if(ec) { std::cerr << ec.message() << std::endl; }
//        std::cout << "Message Received" << std::endl;
//    }
//
//    void on_write(boost::beast::error_code ec) {
//        if(ec) { std::cerr << ec.message() << std::endl; }
//
//    }
//
//    void on_close(boost::beast::error_code ec) {
//        if(ec) { std::cerr << ec.message() << std::endl; }
//        std::cout << "WS Connection Closed" << std::endl;
//    }

//    void
//    on_handshake(boost::beast::error_code ec)
//    {
//        if(ec)
//            std::cerr << ec.message() << std::endl;
//
//        // Send the message
//        ws_.async_write(
//                boost::asio::buffer("herro"),
//                boost::beast::bind_front_handler(
//                        &ClassifierSocket::on_write,
//                        shared_from_this()));
//    }
//
//    void
//    on_write(
//            boost::beast::error_code ec,
//            std::size_t bytes_transferred)
//    {
//        boost::ignore_unused(bytes_transferred);
//
//        if(ec)
//            std::cerr << ec.message() << std::endl;
//
//        // Read a message into our buffer
//        ws_.async_read(
//                buffer_,
//                boost::beast::bind_front_handler(
//                        &ClassifierSocket::on_read,
//                        shared_from_this()));
//    }
//
//    void
//    on_read(
//            boost::beast::error_code ec,
//            std::size_t bytes_transferred)
//    {
//        boost::ignore_unused(bytes_transferred);
//
//        if(ec)
//            std::cerr << ec.message() << std::endl;
//
//        // Close the WebSocket connection
//        ws_.async_close(boost::beast::websocket::close_code::normal,
//                        boost::beast::bind_front_handler(
//                                &ClassifierSocket::on_close,
//                                shared_from_this()));
//    }
//
//    void
//    on_close(boost::beast::error_code ec)
//    {
//        if(ec)
//            std::cerr << ec.message() << std::endl;
//
//        // If we get here then the connection is closed gracefully
//
//        // The make_printable() function helps print a ConstBufferSequence
//        std::cout << boost::beast::make_printable(buffer_.data()) << std::endl;
//    }

public:
    explicit ClassifierSocket(boost::asio::io_context& ioc) : resolver_(ioc), ws_(ioc) {}

    void start(std::string_view host, std::string_view port, std::function<void(std::vector<float>)> positionConsumer) {
        this->dataConsumer = std::move(positionConsumer);

        host_ = host;

        resolver_.async_resolve(
                host,
                port,
                boost::beast::bind_front_handler(
                        &ClassifierSocket::on_resolve,
                        shared_from_this()));
    }
};