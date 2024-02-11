#include <iostream>
#include <memory>
#include "ClassifierSocket.h"


int main() {
    boost::asio::io_context ioc;

    std::shared_ptr<ClassifierSocket> socket = std::make_shared<ClassifierSocket>(ioc);
    socket->start("127.0.0.1", "8080", [&](auto positions) {
        std::cout << positions.size() << std::endl;
    });
    ioc.run();





}
