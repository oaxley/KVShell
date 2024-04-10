/*
 * @file    server.h
 * @brief   Network TCP Server
 */

// ----- guards
#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

// ----- includes
#include "interface.h"

#include <functional>
#include <string>
#include <thread>


// ----- class
namespace Network
{
    using TCPServerCallback = std::function<void(int)>;

    class TCPServer : public Interface
    {
    public:     //< public methods

        TCPServer(std::string address, int port);
        virtual ~TCPServer();

        // no copy semantics
        TCPServer(const TCPServer&) = delete;
        TCPServer operator=(const TCPServer&) = delete;

        // no move semantics
        TCPServer(TCPServer&&) = delete;
        TCPServer& operator=(TCPServer&&) = delete;

        void start();
        void stop();

        void setUserCallback(TCPServerCallback callback);

    private:    //< private methods
        void serveRequest();

    private:    //< private members
        std::thread thread_;            //< execution thread
        bool done_;                     //< execution control variable

        TCPServerCallback callback_;    //< user callback
    };

}

#endif // NETWORK_SERVER_H