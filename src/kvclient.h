/*
 * @file    kvclient.h
 * @brief   Header file for KVClient class
 */

// ----- guards
#ifndef KVCLIENT_H
#define KVCLIENT_H


// ----- includes
#include "application.h"
#include "network/client.h"
#include "vm/defines.h"

#include <queue>
#include <string>


// ----- typedefs
using queue_t = std::queue<VM::QueueItem*>;


// ----- class
class KVClient
{
public:     //< public methods
    KVClient(std::string address, std::string port);
    ~KVClient();

    void parse(Application::CmdLine& cmdline);
    void send();


    // no copy semantics
    KVClient(const KVClient&) = delete;
    KVClient& operator=(const KVClient&) = delete;

    // no move semantics
    KVClient(KVClient&&) = delete;
    KVClient& operator=(KVClient&&) = delete;

private:    //< private methods
    void itemFromArg(std::string_view, VM::Opcodes_t);
    void getKeyName(std::string_view);
    void getValue(std::string_view);

private:    //< private members
    Network::TCPClient* pClient_;
    queue_t items_;
};


#endif // KVCLIENT_H