//
//  Created by Ivan Mejia on 12/24/16.
//
// MIT License
//
// Copyright (c) 2016 ivmeroLabs.
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

#include "basic_controller.hpp"

using namespace cfx;

class MicroserviceController : public BasicController, Controller
{
public:
    MicroserviceController() : BasicController() {}
    ~MicroserviceController() {}


    /*
        pplx::task<json::value> extract_json(bool ignore_content_type = false) const
    {
        auto impl = _m_impl;
        return pplx::create_task(_m_impl->_get_data_available()).then([impl, ignore_content_type](utility::size64_t) {
            return impl->_extract_json(ignore_content_type);
        });
    }
*/
    void handleGet(http_request message) override
    {
        auto path = requestPath(message);
        if (!path.empty())
        {
            if (path[0] == "service" && path[1] == "test")
            {
                auto response = json::value::object();
                response["version"] = json::value::string("0.1.1");
                response["status"] = json::value::string("ready!");
                message.reply(status_codes::OK, response);
            }
        }
        else
        {
            message.reply(status_codes::NotFound);
        }
    }
    void handlePut(http_request message) override
    {
        message.reply(status_codes::NotImplemented, responseNotImpl(methods::PUT));
    }
    void handlePost(http_request message) override
    {
        message.reply(status_codes::NotImplemented, responseNotImpl(methods::POST));
    }
    void handlePatch(http_request message) override
    {
        message.reply(status_codes::NotImplemented, responseNotImpl(methods::PATCH));
    }
    void handleDelete(http_request message) override
    {
        message.reply(status_codes::NotImplemented, responseNotImpl(methods::DEL));
    }
    void handleHead(http_request message) override
    {
        message.reply(status_codes::NotImplemented, responseNotImpl(methods::HEAD));
    }
    void handleOptions(http_request message) override
    {
        message.reply(status_codes::NotImplemented, responseNotImpl(methods::OPTIONS));
    }
    void handleTrace(http_request message) override
    {
        message.reply(status_codes::NotImplemented, responseNotImpl(methods::TRCE));
    }
    void handleConnect(http_request message) override
    {
        message.reply(status_codes::NotImplemented, responseNotImpl(methods::CONNECT));
    }
    void handleMerge(http_request message) override
    {
        message.reply(status_codes::NotImplemented, responseNotImpl(methods::MERGE));
    }
    void initRestOpHandlers() override
    {
        _listener.support(methods::GET, std::bind(&MicroserviceController::handleGet, this, std::placeholders::_1));
        _listener.support(methods::PUT, std::bind(&MicroserviceController::handlePut, this, std::placeholders::_1));
        _listener.support(methods::POST, std::bind(&MicroserviceController::handlePost, this, std::placeholders::_1));
        _listener.support(methods::DEL, std::bind(&MicroserviceController::handleDelete, this, std::placeholders::_1));
        _listener.support(methods::PATCH, std::bind(&MicroserviceController::handlePatch, this, std::placeholders::_1));
    }

private:
    static json::value responseNotImpl(const http::method &method)
    {
        auto response = json::value::object();
        response["serviceName"] = json::value::string("C++ Mircroservice Sample");
        response["http_method"] = json::value::string(method);
        return response;
    }
};