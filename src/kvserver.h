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
#include "vm/defines.h"

#include <string>


// ----- class
class KVServer
{
public:     //< public methods
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

private:    //< private methods
    void freeItems();

private:    //< private members
    KVDbase* pDbase_;
    Network::TCPServer* pServer_;
    bool done_;
    VM::queue_t items_;
};


#endif // KVSERVER_H
