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
        removeItem();
    }
}

// get the next item from the queue
VM::QueueItem* KVServer::nextItem()
{
    return items_.front();
}

// remove the item from the queue
void KVServer::removeItem()
{
    auto* item = items_.front();
    items_.pop();
    delete item;
}


// network callback
void KVServer::callback(int sock)
{
    // recreate the items
    std::uint8_t buffer[Constants::Network::Protocol::max_read_buffer] = {0};

    // read the Start-of-Transmission character
    int n = recv(sock, buffer, sizeof(std::uint8_t), 0);

    if (buffer[0] != Constants::Network::Protocol::sot) {
        std::cerr << "Error: unable to find the SOT marker! [" << std::hex << buffer[0] << "]\n";
        // purge the socket
        while(n > 0) {
            n = recv(sock, buffer, Constants::Network::Protocol::max_read_buffer, 0);
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
    std::uint8_t* key{nullptr};
    std::uint8_t* value{nullptr};
    int uid{0};
    int ksize{0};
    int vsize{0};
    DBResult* pResult{nullptr};

    // retrieve the opcode
    VM::Opcodes_t opcode = nextItem()->opcode;
    removeItem();

    // retrieve the UID
    uid = VM::getUID(nextItem());
    removeItem();

    // retrieve the KEY
    if (nextItem()->opcode == VM::Opcodes_t::K_NAME) {
        key = retrieveKey(&ksize);
    }

    switch(opcode)
    {
        case VM::Opcodes_t::OP_GET:     // retrieve a value from the DB
            {
                // retrieve the result
                pResult = pDbase_->fetchRow(key, ksize, uid);
                if (pResult != nullptr) {
                    createResponse(VM::Opcodes_t::R_VALUE, pResult);
                } else {
                    createResponse(VM::Opcodes_t::R_ERROR, std::string("Error: unable to retrieve data with the key provided!"));
                }
            }
            break;

        case VM::Opcodes_t::OP_SET:     // set a value in the DB
            {
                // retrieve the value
                value = retrieveValue(&vsize);

                if (pDbase_->insert(key, ksize, value, vsize, uid) == 0) {
                    createResponse(VM::Opcodes_t::R_ERROR, std::string("Error: unable to insert data with the key provided!"));
                } else {
                    createResponse(VM::Opcodes_t::R_VALUE, std::string("OK"));
                }
            }
            break;

        case VM::Opcodes_t::OP_EXPDT:   // expiry with a datetime
            break;

        case VM::Opcodes_t::OP_EXPDR:   // expiry with a duration
            break;

        case VM::Opcodes_t::OP_DEL:     // delete a key
            {
                bool result = pDbase_->remove(key, ksize, uid);
                if (result) {
                    createResponse(VM::Opcodes_t::V_VALUE, std::string("OK"));
                } else {
                    createResponse(VM::Opcodes_t::R_ERROR, std::string("Error: unable to delete the key!"));
                }
            }
            break;

        case VM::Opcodes_t::OP_PRT:     // print key with a regexp
            break;

        case VM::Opcodes_t::OP_EXIST:   // check for a key
            {
                bool result = pDbase_->exists(key, ksize, uid);
                if (result) {
                    createResponse(VM::Opcodes_t::V_VALUE, "True");
                } else {
                    createResponse(VM::Opcodes_t::V_VALUE, "False");
                }
            }
            break;
    }

    // free memory
    delete [] key;
    delete [] value;
    delete pResult;
}

// send the response to the user
void KVServer::sendResponse(int sock)
{
    // send start of transmission
    send(sock, &Constants::Network::Protocol::sot, 1, 0);

    // send all the blocks
    while (!items_.empty())
    {
        std::uint8_t value{};

        // retrieve the item
        auto* item = items_.front();

        // send the opcode
        value = static_cast<std::uint8_t>(item->opcode);
        send(sock, &value, sizeof(value), 0);

        // retrieve the size + value
        if (item->szdata == 0) {
            std::uint16_t zero = 0;
            send(sock, reinterpret_cast<std::uint8_t*>(&zero), sizeof(zero), 0);
        } else {
            send(sock, reinterpret_cast<std::uint8_t*>(&item->szdata), sizeof(item->szdata), 0);
            send(sock, item->pdata, item->szdata, 0);
        }

        // next item
        items_.pop();
        delete item;
    }

    // send end of transmission
    send(sock, &Constants::Network::Protocol::eot, 1, 0);
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

// retrieve the Key from the queue by aggregating multiple K_NAME blocks
std::uint8_t* KVServer::retrieveKey(int* size)
{
    return retrieveData(size, VM::Opcodes_t::K_NAME);
}

// retrieve the Value from the queue by aggregating multiple V_VALUE blocks
std::uint8_t* KVServer::retrieveValue(int* size)
{
    return retrieveData(size, VM::Opcodes_t::V_VALUE);
}


// create a response from a std::uint8_t pointer
// TO BE DONE
void KVServer::createResponse(VM::Opcodes_t code, std::uint8_t* pData, int size)
{
    // delete the remaining item in the queue
    // at this point they are not needed anymore
    freeItems();
}

// create a response from a DB result
// TODO:
// DBResult has already allocated memory for the result
// we can use this and avoid re-allocated a second time the memory
// just to release it again later on
void KVServer::createResponse(VM::Opcodes_t code, DBResult* pResult)
{
    // delete the remaining item in the queue
    // at this point they are not needed anymore
    freeItems();

    // only create block of regular size
    int item_size = pResult->size;
    int count = 0;

    while (count < item_size)
    {
        int remaining = item_size - count;

        int block_size = 0;
        if (remaining > Constants::Network::Protocol::max_item_size) {
            block_size = Constants::Network::Protocol::max_item_size;
        } else {
            block_size = remaining;
        }

        // create a new block
        VM::QueueItem* item = new VM::QueueItem {
            opcode: code,
            szdata: static_cast<std::uint16_t>(block_size),
            pdata: new std::uint8_t[block_size]
        };

        // copy the data to the block (to find a clever way to do it)
        memcpy(item->pdata, pResult->pData+count, block_size);
        count += block_size;

        items_.push(item);
    }
}

// create a response with a simple string message
void KVServer::createResponse(VM::Opcodes_t code, std::string msg)
{
    // delete the remaining item in the queue
    // at this point they are not needed anymore
    freeItems();

    // create the new item
    VM::QueueItem* item = new VM::QueueItem {
        opcode: code,
        szdata: std::size(msg),
        pdata: new std::uint8_t[std::size(msg)]
    };

    // copy the message
    memcpy(item->pdata, msg.data(), std::size(msg));

    // add the item to the queue
    items_.push(item);
}