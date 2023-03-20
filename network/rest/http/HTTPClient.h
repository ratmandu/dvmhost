/**
* Digital Voice Modem - Host Software
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Host Software
*
*/
/*
*   Copyright (C) 2023 by Bryan Biedenkapp N2PLL
*
*   Permission is hereby granted, free of charge, to any person or organization 
*   obtaining a copy of the software and accompanying documentation covered by 
*   this license (the “Software”) to use, reproduce, display, distribute, execute, 
*   and transmit the Software, and to prepare derivative works of the Software, and
*   to permit third-parties to whom the Software is furnished to do so, all subject
*   to the following:
*
*   The copyright notices in the Software and this entire statement, including the
*   above license grant, this restriction and the following disclaimer, must be included
*   in all copies of the Software, in whole or in part, and all derivative works of the
*   Software, unless such copies or derivative works are solely in the form of
*   machine-executable object code generated by a source language processor.
*
*   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
*   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
*   PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE
*   DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
*   CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
*   OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#if !defined(__REST_HTTP__HTTP_CLIENT_H__)
#define __REST_HTTP__HTTP_CLIENT_H__

#include "Defines.h"
#include "network/rest/http/Connection.h"
#include "network/rest/http/ConnectionManager.h"
#include "network/rest/http/HTTPRequestHandler.h"
#include "Thread.h"

#include <asio.hpp>

#include <thread>
#include <string>
#include <signal.h>
#include <utility>
#include <memory>
#include <mutex>

namespace network 
{
    namespace rest 
    {
        namespace http 
        {

            // ---------------------------------------------------------------------------
            //  Class Declaration
            //      This class implements top-level routines of the HTTP client.
            // ---------------------------------------------------------------------------

            template<typename RequestHandlerType, template<class> class ConnectionImpl = Connection>
            class HTTPClient : private Thread {
            public:
                /// <summary>Initializes a new instance of the HTTPClient class.</summary>
                explicit HTTPClient(const std::string& address, uint16_t port) :
                    m_address(address),
                    m_port(port),
                    m_connection(nullptr),
                    m_ioContext(), 
                    m_connectionManager(),
                    m_socket(m_ioContext), 
                    m_requestHandler() 
                { 
                    /* stub */ 
                }
                /// <summary>Initializes a copy instance of the HTTPClient class.</summary>
                HTTPClient(const HTTPClient&) = delete;
    
                /// <summary></summary>
                HTTPClient& operator=(const HTTPClient&) = delete;

                /// <summary>Helper to set the HTTP request handlers.</summary>
                template<typename Handler>
                void setHandler(Handler&& handler)
                {
                    m_requestHandler = RequestHandlerType(std::forward<Handler>(handler));
                }

                /// <summary>Send HTTP request to HTTP server.</summary>
                void request(HTTPPayload& request)
                {
                    asio::post(m_ioContext, [this, request]() {
                        std::lock_guard<std::mutex> guard(m_lock);
                        {
                            if (m_connection != nullptr) {
                                m_connection->send(request);
                            }
                        }
                    });
                }

                /// <summary>Opens connection to the network.</summary>
                bool open()
                {
                    return run();
                }

                /// <summary>Closes connection to the network.</summary>
                void close()
                {
                    if (m_connection != nullptr) {
                        m_connection->stop();
                    }

                    wait();
                }

            private:
                /// <summary></summary>
                virtual void entry()
                {
                    asio::ip::tcp::resolver resolver(m_ioContext);
                    auto endpoints = resolver.resolve(m_address, std::to_string(m_port));

                    connect(endpoints);

                    // the entry() call will block until all asynchronous operations
                    // have finished
                    m_ioContext.run();
                }

                /// <summary>Perform an asynchronous connect operation.</summary>
                void connect(asio::ip::basic_resolver_results<asio::ip::tcp>& endpoints)
                {
                    asio::connect(m_socket, endpoints);
                    m_connection = std::make_shared<ConnectionType>(std::move(m_socket), m_connectionManager, m_requestHandler, false, true);
                    m_connection->start();
                }

                std::string m_address;
                uint16_t m_port;

                typedef ConnectionImpl<RequestHandlerType> ConnectionType;
                typedef std::shared_ptr<ConnectionType> ConnectionTypePtr;

                ConnectionTypePtr m_connection;

                asio::io_context m_ioContext;

                ConnectionManager<ConnectionTypePtr> m_connectionManager;

                asio::ip::tcp::socket m_socket;
                
                RequestHandlerType m_requestHandler;

                std::mutex m_lock;
            };
        } // namespace http
    } // namespace rest
} // namespace network
 
#endif // __REST_HTTP__HTTP_CLIENT_H__
