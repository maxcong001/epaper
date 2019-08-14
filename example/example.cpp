
#include "logger/boost_logger.hpp"

#include <stdlib.h>
#include <stdio.h>
#include "epaper/epd7in5bpaint.hpp"
#include "epaper/imagedata.hpp"
#include <bcm2835.h>
#include "epaper/fonts.hpp"

#include <iostream>

#include <usr_interrupt_handler.hpp>
#include <runtime_utils.hpp>

#include "microsvc_controller.hpp"

using namespace web;
using namespace cfx;

int main(int argc, const char *argv[])
{
    InterruptHandler::hookSIGINT();
    // epaper init
    Paint *globPaint = Paint::getInstance();

    if (globPaint->init() == -1)
    {
        return 0;
    }

    MicroserviceController server;
    server.setEndpoint("http://host_auto_ip4:6502/v1/epaper/api");

    try
    {
        // wait for server initialization...
        server.accept().wait();
        std::cout << "Modern C++ Microservice now listening for requests at: " << server.endpoint() << '\n';

        InterruptHandler::waitForUserInterrupt();

        server.shutdown().wait();
    }
    catch (std::exception &e)
    {
        std::cerr << "something wrong happen! :(" << '\n';
    }
    catch (...)
    {
        RuntimeUtils::printStackTrace();
    }

    return 0;
}
