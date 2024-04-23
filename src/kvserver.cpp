/*
 * @file    kvserver.cpp
 * @brief   Source file for the KVServer class
 */

// ----- includes
#include "constants.h"
#include "kvserver.h"

#include <signal.h>
#include <sys/socket.h>

#include <chrono>
#include <functional>
#include <iostream>


// ----- functions
std::function<void(int)> KVServerCallback;

void signalHandler(int signal)
{
    KVServerCallback(signal);
}


// ----- class

// constructor
KVServer::KVServer(std::string address, std::string port, std::string dbname) :
    pDbase_{nullptr}, pServer_{nullptr}, done_{true}
{
    // create a new database instance
    pDbase_ = new KVDbase(dbname);
    if (!pDbase_) {
        std::cerr << "Error: unable to create a KVDbase instance!\n";
        std::exit(EXIT_FAILURE);
    }

    // create a new TCPServer
    pServer_ = new Network::TCPServer{address, port};
    if (!pServer_) {
        std::cerr << "Error: unable to create a TCPServer instance!\n";
        std::exit(EXIT_FAILURE);
    }
}

// destructor
KVServer::~KVServer()
{
    // stop properly the server
    stop();
    delete pServer_;
    pServer_ = nullptr;

    // free the items in the queue if any
    freeItems();
}

// start the server
void KVServer::start()
{
    // install the signal handler for CTRL+C
    using namespace std::placeholders;
    KVServerCallback = std::bind(&KVServer::signalHandler, this, _1);
    signal(SIGINT, ::signalHandler);

    // set the TCPServer callback via Lambda function
    auto fcn = [this](int sock) { this->callback(sock); };
    pServer_->setUserCallback(fcn);

    // start the TCP server
    pServer_->start();

    // infinite mainloop
    std::cerr << "Starting KVServer mainloop... CTRL+C to stop\n";
    done_ = false;
    while (!done_)
    {
        // wait for 200ms
        std::this_thread::sleep_for(Constants::KVServer::kvserver_mainloop_timeout);
    }
}

// stop the server
void KVServer::stop()
{
    if (!done_) {
        done_ = true;
        pServer_->stop();
    }
}

// signal handler
void KVServer::signalHandler(int signal)
{
    if (signal == SIGINT) {
        stop();
    }
}

// free the items in the queue (if any)
void KVServer::freeItems()
{
    while (items_.size() > 0) {
        auto* elt = items_.front();
        items_.pop();
        delete elt;
    }
}

// network callback
void KVServer::callback(int sock)
{
    char buffer[1024] = {0};
    int n = recv(sock, buffer, 1024, 0);
    std::cerr << "Received " << n << " bytes : " << buffer << "\n";
}

// signal handler
void KVServer::signalHandler(int signal)
{
    if (signal == SIGINT) {
        stop();
    }
}
