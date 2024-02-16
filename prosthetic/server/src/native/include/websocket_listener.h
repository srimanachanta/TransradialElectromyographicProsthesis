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

#include "websocket_session.h"

// Accepts incoming connections and launches the sessions
class websocket_listener : public std::enable_shared_from_this<websocket_listener> {
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

public:
    websocket_listener(net::io_context& ioc, const tcp::endpoint& endpoint) : ioc_(ioc), acceptor_(ioc) {
        beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if(ec) {
            fail(ec, "open");
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if(ec) {
            fail(ec, "set_option");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if(ec) {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if(ec) {
            fail(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void run() {
        acceptor_.async_accept(
                net::make_strand(ioc_),
                beast::bind_front_handler(
                        &websocket_listener::on_accept,
                        shared_from_this()));
    }

private:
    void on_accept(beast::error_code ec, tcp::socket socket) {
        if(ec) {
            fail(ec, "accept");
        } else {
            // Create the session and run it
            std::make_shared<websocket_session>(std::move(socket))->run();
        }

        // Accept another connection
        acceptor_.async_accept(
                net::make_strand(ioc_),
                beast::bind_front_handler(
                        &websocket_listener::on_accept,
                        shared_from_this()));
    }
};