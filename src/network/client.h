/*
 * @file    client.h
 * @brief   Header file for Network TCP Client class
 */

// ----- guards
#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

// ----- includes
#include "interface.h"

#include <cstdint>

// ----- class
namespace Network
{
    class TCPClient : public Interface
    {
    public:
        TCPClient(std::string address, std::string port);
        virtual ~TCPClient();

        void connect();
        void send(std::uint8_t*, int n);
        int recv(std::uint8_t*, int n);


        // no copy semantics
        TCPClient(const TCPClient&) = delete;
        TCPClient& operator=(const TCPClient&) = delete;

        // no move semantics
        TCPClient(TCPClient&&) = delete;
        TCPClient& operator=(TCPClient&&) = delete;


    };

} //< end namespace

#endif // NETWORK_CLIENT_H