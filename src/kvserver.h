/*
 * @file    kvserver.h
 * @brief   Header file for the KVServer class
 */

// ----- guards
#ifndef KVSERVER_H
#define KVSERVER_H

// ----- includes
#include "kvdbase.h"
#include "network.h"

#include <string>


// ----- class
class KVServer
{
public:
    KVServer(std::string address, std::string port, std::string dbname);
    ~KVServer();

    void start();
    void stop();

    void callback(int sock);
    void signalHandler(int signal);

    // no copy
    KVServer(const KVServer&) = delete;
    KVServer operator=(const KVServer&) = delete;

    // no move semantics
    KVServer(KVServer&&) = delete;
    KVServer& operator=(KVServer&&) = delete;

private:
    KVDbase* pDbase_;
    Network::TCPServer* pServer_;
    bool done_;
};


#endif // KVSERVER_H
