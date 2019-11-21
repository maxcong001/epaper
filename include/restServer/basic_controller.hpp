//
//  Created by Ivan Mejia on 12/03/16.
//
// MIT License
//
// Copyright (c) 2016 ivmeroLabs. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include <string>
#include <cpprest/http_listener.h>
#include "restServer/network_utils.hpp"
#include <pplx/pplxtasks.h>
#include "controller.hpp"
#include "logger/logger.hpp"

using namespace web;
using namespace http::experimental::listener;

namespace cfx
{
class BasicController
{
protected:
    http_listener _listener; // main micro service network endpoint

public:
    BasicController() = default;
    ~BasicController() = default;

    void setEndpoint(const std::string &value)
    {
        uri endpointURI(value);
        uri_builder endpointBuilder;

        endpointBuilder.set_scheme(endpointURI.scheme());
        if (endpointURI.host() == "host_auto_ip4")
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "rest server will listen on : " << NetworkUtils::hostIP4());
            }
            endpointBuilder.set_host(NetworkUtils::hostIP4());
            //endpointBuilder.set_host("192.168.31.120");
        }
        else if (endpointURI.host() == "host_auto_ip6")
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "rest server will listen on : " << NetworkUtils::hostIP6());
            }
            endpointBuilder.set_host(NetworkUtils::hostIP6());
        }
        else
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "invalid host type : " << endpointURI.host() << "will use IPv4 as default"
                                                    << ", IP is : " << NetworkUtils::hostIP4());
            }
            endpointBuilder.set_host(NetworkUtils::hostIP4());
        }
        endpointBuilder.set_port(endpointURI.port());
        endpointBuilder.set_path(endpointURI.path());
        /* https related
        http_listener_config conf;
        conf.set_ssl_context_callback([](boost::asio::ssl::context &ctx) {
            ctx.set_options(boost::asio::ssl::context::default_workarounds);

            // Password callback needs to be set before setting cert and key.
            ctx.set_password_callback([](std::size_t max_length, boost::asio::ssl::context::password_purpose purpose) {
                return "password";
            });

            ctx.use_certificate_file("cert.pem", boost::asio::ssl::context::pem);
            ctx.use_private_key_file("key.pem", boost::asio::ssl::context::pem);
            ctx.use_certificate_chain_file("chain.pem");
        });
*/

        _listener = http_listener(endpointBuilder.to_uri());
        //_listener = http_listener(endpointBuilder.to_uri(), conf);
        // _listener = http_listener("https://10.242.146.124:6502", conf);
    }
    std::string endpoint() const
    {
        return _listener.uri().to_string();
    }
    pplx::task<void> accept()
    {
        initRestOpHandlers();
        return _listener.open();
    }
    pplx::task<void> shutdown()
    {
        return _listener.close();
    }

    virtual void initRestOpHandlers()
    {
        /* had to be implemented by the child class */
    }

    std::vector<utility::string_t> requestPath(const http_request &message)
    {
        auto relativePath = uri::decode(message.relative_uri().path());
        return uri::split_path(relativePath);
    }
};
} // namespace cfx