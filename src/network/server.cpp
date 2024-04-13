/*
 * @file    server.cpp
 * @brief   Source file for Network TCP Server class
 */

// ----- includes
#include "../application.h"
#include "../constants.h"
#include "server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>


namespace Network
{

// ----- class

TCPServer::TCPServer(std::string address, std::string port) :
    Interface(address, port), thread_{}, done_{true}, callback_{nullptr}
{
    // bind the socket
    bindSocket();
}

TCPServer::~TCPServer()
{
    // wait for the thread
    if (thread_.joinable()) {
        std::cerr << "Waiting for Thread to finish...\n";
        done_ = true;
        thread_.join();
    }

    if (socket_ > 0) {
        close(socket_);
        socket_ = -1;
    }
}

// bind the socket
void TCPServer::bindSocket()
{
    // create the server socket
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ < 0) {
        std::cerr << "Error: unable to create the server socket!\n";
        std::exit(EXIT_FAILURE);
    }

    // prepare the structure for bind
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;

    // lookup the proper value for the port (in case user provided a service name)
    int service = resolvePort(port_);
    if (service == -1) {
        std::cerr << "Error: invalid TCP service provided!\n";
        std::exit(EXIT_FAILURE);
    }
    server_address.sin_port = service;

    // use 0.0.0.0 for now
    void *ip = resolveAddr(address_);
    if (ip == nullptr) {
        server_address.sin_addr.s_addr = INADDR_ANY;
    } else {
        struct in_addr* v = reinterpret_cast<struct in_addr*>(ip);
        server_address.sin_addr.s_addr = v->s_addr;
    }

    // set the REUSE flag
    int reuse = 1;
    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
        std::cerr << "Error: unable to set the REUSE flag on the socket\n";
        std::exit(EXIT_FAILURE);
    }

    // bind
    bind(socket_, (struct sockaddr*)&server_address, sizeof(server_address));

    // listen
    listen(socket_, Constants::Network::server_listen_max);
}

// serve the request from client
void TCPServer::setUserCallback(TCPServerCallback callback)
{
    callback_ = callback;
}

// start the server
void TCPServer::start()
{
    // start the serving thread
    std::cerr << "Starting TCP Thread...\n";
    done_ = false;
    thread_ = std::thread(&TCPServer::serveRequest, this);
}

// stop the server
void TCPServer::stop()
{
    if (!done_) {
        std::cerr << "Stopping TCP Thread...\n";
        done_ = true;
        if (thread_.joinable()) {
            thread_.join();
        }
    }
}

// main thread function to serve requests
void TCPServer::serveRequest()
{
    // create epoll instance
    struct epoll_event event, events[Constants::Network::epoll_max_events];
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        std::cerr << "Error: unable to create the epoll instance!\n";
        std::exit(EXIT_FAILURE);
    }

    // add the server socket to the monitoring list
    event.events = EPOLLIN;
    event.data.fd = socket_;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_, &event) < 0) {
        std::cerr << "Error: unable to add server socket to the epoll instance!\n";
        std::exit(EXIT_FAILURE);
    }

    // mainloop
    while (!done_)
    {
        // wait for an event (or timeout)
        int num_events = epoll_wait(epoll_fd, events, Constants::Network::epoll_max_events, Constants::Network::epoll_timeout);
        if (num_events == -1) {
            std::cerr << "Error: epoll unable to wait for events!\n";
            std::exit(EXIT_FAILURE);
        }

        // go through all the event
        for (int i = 0; i < num_events; ++i)
        {
            // new incomming connection
            if (events[i].data.fd == socket_)
            {
                struct sockaddr_in client;
                socklen_t length = sizeof(client);

                int sock = accept(socket_, (struct sockaddr*) &client, &length);
                if (sock < 0) {
                    std::cerr << "Error: unable to accept incoming connection!\n";
                    continue;
                }

                std::cerr << "New connection from " << inet_ntoa(client.sin_addr);
                std::cerr << ":" << ntohs(client.sin_port) << "\n";

                // call the user callback if it's defined
                if (callback_)
                    callback_(sock);

                // close the connection
                close(sock);
            }
        }
    }
}

}   //< end namespace
