/*
 * @file    kvserver.cpp
 * @brief   Source file for the KVServer class
 */

// ----- includes
#include "constants.h"
#include "kvserver.h"
#include "vm/helpers.h"

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
    // recreate the items
    std::uint8_t buffer[Constants::KVServer::max_read_buffer] = {0};

    // read the Start-of-Transmission character
    int n = recv(sock, buffer, sizeof(std::uint8_t), 0);

    if (buffer[0] != Constants::Network::Protocol::sot) {
        std::cerr << "Error: unable to find the SOT marker! [" << std::hex << buffer[0] << "]\n";
        // purge the socket
        while(n > 0) {
            n = recv(sock, buffer, Constants::KVServer::max_read_buffer, 0);
        }
        return;
    }

    // wait for the End-of-Transmission character
    while (true)
    {
        // read the next character
        n = recv(sock, buffer, sizeof(std::uint8_t), 0);
        if ((buffer[0] == Constants::Network::Protocol::eot) || (n <= 0)) {
            break;
        }

        // this is an opcode
        VM::Opcodes_t op = static_cast<VM::Opcodes_t>(buffer[0]);

        // retrieve the size of the data
        recv(sock, buffer, sizeof(std::uint16_t), 0);
        std::uint16_t* p = reinterpret_cast<std::uint16_t*>(buffer);

        // create a new item
        VM::QueueItem* item = nullptr;

        if (*p == 0) {
            item = new VM::QueueItem {
                opcode: op,
                szdata: 0,
                pdata: nullptr
            };
            items_.push(item);
        } else {
            item = new VM::QueueItem {
                opcode: op,
                szdata: *p,
                pdata: new std::uint8_t[(*p) + 1]
            };

            // read the data
            memset(item->pdata, 0, (*p) + 1);
            recv(sock, item->pdata, *p, 0);
            items_.push(item);
        }
    }

    // interpret the command from the user
    processCommand();

    // send the response to the user
    sendResponse(sock);

    // release the items in the queue
    freeItems();
}

// process the command from the user
void KVServer::processCommand()
{
}

// send the response to the user
void KVServer::sendResponse(int sock)
{

}

// get the next item from the queue
VM::QueueItem* KVServer::next()
{
    auto* item = items_.front();
    items_.pop();

    return item;
}


// retrieve the data from an item block
std::uint8_t* KVServer::retrieveData(int* size, VM::Opcodes_t opcode)
{
    std::uint8_t* value = nullptr;
    std::uint16_t total_size{0};

    while(!items_.empty())
    {
        // retrieve the next element from the queue (but don't remove it yet)
        auto* item = items_.front();

        // no longer a K_NAME element
        if (item->opcode != opcode)
            break;

        // retrieve the data from the block
        std::uint16_t size{0};
        std::uint8_t* data = VM::getData(item, &size);

        // initial block
        if (value == nullptr) {
            value = data;
            total_size = size;
        } else {
            // we need to move the data elsewhere ...
            int new_size = total_size + size + 1;
            std::uint8_t* new_ptr = new std::uint8_t[new_size];

            // copy the data
            memset(new_ptr, 0, new_size);
            memcpy(new_ptr, value, total_size);
            memcpy(new_ptr + total_size, data, size);

            // compute the new size
            total_size = total_size + size;

            // delete previous buffer
            delete [] value;
            delete [] data;

            // this is the new value
            value = new_ptr;
        }

        // remove the element from the queue
        items_.pop();
        delete item;
    }

    *size = total_size;
    return value;
}

// retrieve the Key from the queue by aggregating multiple K_NAME block
std::uint8_t* KVServer::retrieveKey(int* size)
{
    return retrieveData(size, VM::Opcodes_t::K_NAME);
}


std::uint8_t* KVServer::retrieveValue(int* size)
{
    return retrieveData(size, VM::Opcodes_t::V_VALUE);
}




// ----- database operations

// retrieve a value from the database
void KVServer::getDBValue(std::uint8_t* key, int size,  int uid)
{

    try
    {
        SQLite::Database& db = pDbase_->get();

        std::cerr << "Prepare statement...\n";
        SQLite::Statement query(db, "SELECT value FROM KVEntry WHERE user = :uid AND key = :key");

        std::cerr << "Binding values to the query...\n";
        query.bind(":uid", uid);
        query.bind(":key", key, size);

        std::cerr << "Statement: " << query.getExpandedSQL() << "\n";

        std::cerr << "Get results...\n";
        while (query.executeStep())
        {
            std::cerr << query.getColumn(0) << "\n";
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
}

}