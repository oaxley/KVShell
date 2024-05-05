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
    void processCommand();
    void sendResponse(int sock);

    void createResponse(VM::Opcodes_t code, std::uint8_t* pData, int size);
    void createResponse(VM::Opcodes_t code, DBResult* pResult);
    void createResponse(VM::Opcodes_t code, std::string msg);

    // queue management
    void freeItems();               //< remove all the items from the queue
    VM::QueueItem* nextItem();      //< return the value in front of the queue (but don't remove it)
    void removeItem();              //< remove the value in front of the queue


    std::uint8_t* retrieveData(int* size, VM::Opcodes_t opcode);
    std::uint8_t* retrieveKey(int* size);
    std::uint8_t* retrieveValue(int* size);


private:    //< private members
    KVDbase* pDbase_;
    Network::TCPServer* pServer_;
    bool done_;
    VM::queue_t items_;
};


#endif // KVSERVER_H
