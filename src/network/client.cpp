/*
 * @file    client.cpp
 * @brief   Source file for Network TCP Client class
 */

// ----- includes
#include "client.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>


namespace Network
{

// ----- methods
TCPClient::TCPClient(std::string address, std::string port) :
    Interface{address, port}
{
    // create the socket
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ < 0) {
        std::cerr << "Error: unable to create the client socket!\n";
        std::exit(EXIT_FAILURE);
    }
}

/*virtual*/ TCPClient::~TCPClient()
{
    if (socket_ > 0) {
        close(socket_);
        socket_ = -1;
    }
}

// connect to the server
void TCPClient::connect()
{
    // connection structure
    sockaddr_in s;
    s.sin_family = AF_INET;

    // lookup the proper value for the port (in case user provide a service name)
    int service = resolvePort(port_);
    if (service == -1) {
        std::cerr << "Error: invalid TCP service provided!\n";
        std::exit(EXIT_FAILURE);
    }
    s.sin_port = htons(service);

    // resolve the address
    void *ip = resolveAddr(address_);
    if (ip == nullptr) {
        s.sin_addr.s_addr = INADDR_ANY;
    } else {
        struct in_addr* v = reinterpret_cast<struct in_addr*>(ip);
        s.sin_addr.s_addr = v->s_addr;
    }

    // connect to the server
    if (::connect(socket_, (struct sockaddr*)&s, sizeof(s)) < 0) {
        std::cerr << "Error: unable to connect to server [" << address_ << ":" << port_ << "]\n";
        std::exit(EXIT_FAILURE);
    }
}

// send data to the server
void TCPClient::send(std::uint8_t *pData, int size)
{
    int res = ::send(socket_, pData, size, 0);
    if (res != size) {
        std::cerr << "Error: only " << res << " bytes sent out of " << size << "\n";
        std::exit(EXIT_FAILURE);
    }
}

// recv data from the server
int TCPClient::recv(std::uint8_t *pData, int n)
{
    int res = ::recv(socket_, pData, n, 0);
    return res;
}

} //< end namespace