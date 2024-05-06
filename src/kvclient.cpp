/*
 * @file    kvclient.h
 * @brief   Source file for KVClient class
 */

// ----- includes
#include "constants.h"
#include "kvclient.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include <iostream>


// ----- class

// constructor
KVClient::KVClient(std::string address, std::string port) :
    pClient_{nullptr}
{
    // create a new TCPClient
    pClient_ = new Network::TCPClient(address, port);
    if (!pClient_) {
        std::cerr << "Error: unable to create a TCPClient instance\n";
        std::exit(EXIT_FAILURE);
    }
}

// destructor
KVClient::~KVClient()
{
    // delete all the remaining items in the queue
    while (items_.size() > 0) {
        auto* elt = items_.front();
        items_.pop();

        delete elt;
    }

    // delete the client
    delete pClient_;
    pClient_ = nullptr;
}

// set user UID/GID
void KVClient::setUser(int uid, int gid)
{
    uid_ = uid;
    gid_ = gid;
}

// parse the command line and create the linked list
void KVClient::parse(Application::CmdLine& cmdline)
{
    Application::CmdLine::Args_t::const_iterator it = cmdline.args().cbegin();
    Application::CmdLine::Args_t::const_iterator end = cmdline.args().cend();

    int args_size = cmdline.args().size();

    while (it != end)
    {
        // set a new value
        if ((*it).compare("set") == 0) {
            VM::QueueItem* item = new VM::QueueItem {
                opcode: VM::Opcodes_t::OP_SET,
                szdata: 0,
                pdata: nullptr
            };
            items_.push(item);
            ++it;

            // add the userId
            std::uint8_t* user_id = new std::uint8_t[sizeof(uid_)];
            memcpy(user_id, &uid_, sizeof(uid_));
            item = new VM::QueueItem {
                opcode: VM::Opcodes_t::U_USER,
                szdata: sizeof(uid_),
                pdata: user_id
            };
            items_.push(item);

            // read the Key Name
            getKeyName(*(it++));

            // read the value
            if (args_size == 2) {
                getValue("");
            } else {
                getValue(*(it++));
            }

            break;
        }

        // get a value
        if ((*it).compare("get") == 0) {
            VM::QueueItem* item = new VM::QueueItem {
                opcode: VM::Opcodes_t::OP_GET,
                szdata: 0,
                pdata: nullptr
            };
            items_.push(item);
            ++it;

            // add the userId
            std::uint8_t* user_id = new std::uint8_t[sizeof(uid_)];
            memcpy(user_id, &uid_, sizeof(uid_));
            item = new VM::QueueItem {
                opcode: VM::Opcodes_t::U_USER,
                szdata: sizeof(uid_),
                pdata: user_id
            };
            items_.push(item);

            // read the key name
            getKeyName(*(it++));

            break;
        }

        // delete a key
        if ((*it).compare("delete") == 0) {
            VM::QueueItem* item = new VM::QueueItem {
                opcode: VM::Opcodes_t::OP_DEL,
                szdata: 0,
                pdata: nullptr
            };
            items_.push(item);
            ++it;

            // add the userId
            std::uint8_t* user_id = new std::uint8_t[sizeof(uid_)];
            memcpy(user_id, &uid_, sizeof(uid_));
            item = new VM::QueueItem {
                opcode: VM::Opcodes_t::U_USER,
                szdata: sizeof(uid_),
                pdata: user_id
            };
            items_.push(item);

            // read the key name
            getKeyName(*(it++));

            break;
        }

        // check if a key exists
        if ((*it).compare("exists") == 0) {
            VM::QueueItem* item = new VM::QueueItem {
                opcode: VM::Opcodes_t::OP_EXIST,
                szdata: 0,
                pdata: nullptr
            };
            items_.push(item);
            ++it;

            // add the userId
            std::uint8_t* user_id = new std::uint8_t[sizeof(uid_)];
            memcpy(user_id, &uid_, sizeof(uid_));
            item = new VM::QueueItem {
                opcode: VM::Opcodes_t::U_USER,
                szdata: sizeof(uid_),
                pdata: user_id
            };
            items_.push(item);

            // read the key name
            getKeyName(*(it++));

            break;
        }

    }
}

// create an item from an args (Name or Value)
void KVClient::itemFromArg(std::string_view arg, VM::Opcodes_t opcode)
{
    const char* c = arg.data();
    int item_size = arg.size();
    int count = 0;

    while (count < item_size)
    {
        int remaining = item_size - count;

        // ensure we create only block of 64KiB
        int block_size = 0;
        if (remaining > Constants::Network::Protocol::max_item_size) {
            block_size = Constants::Network::Protocol::max_item_size;
        } else {
            block_size = remaining;
        }

        // create a new block
        VM::QueueItem* item = new VM::QueueItem {
            opcode: opcode,
            szdata: static_cast<std::uint16_t>(block_size),
            pdata: new std::uint8_t[block_size]
        };

        // copy the string to the block
        memcpy(item->pdata, c + count, block_size);
        count += block_size;

        // add the block to the queue
        items_.push(item);
    }
}


// return the Key Name from the arguments
void KVClient::getKeyName(std::string_view arg)
{
    itemFromArg(arg, VM::Opcodes_t::K_NAME);
}

void KVClient::getValue(std::string_view arg)
{
    // data from the command line
    if (arg.size() > 0) {
        itemFromArg(arg, VM::Opcodes_t::V_VALUE);
        return;
    }

    // data are passed from STDIN either with "<" or a pipe "|"
    if (!isatty(fileno(stdin))) {
        std::uint8_t buffer[Constants::Network::Protocol::max_item_size];
        while (true)
        {
            int n = read(fileno(stdin), buffer, Constants::Network::Protocol::max_item_size);

            // nothing to read anymore
            if (n <= 0)
                break;

            // create a new block
            VM::QueueItem* item = new VM::QueueItem {
                opcode: VM::Opcodes_t::V_VALUE,
                szdata: static_cast<std::uint16_t>(n),
                pdata: new std::uint8_t[n]
            };

            // copy the data to the block
            memcpy(item->pdata, buffer, n);

            items_.push(item);
        }
    }
}

// send the command to the server
void KVClient::send()
{
    // connect to the server
    pClient_->connect();

    // send the start of transmission
    pClient_->send(&Constants::Network::Protocol::sot, 1);

    while (!items_.empty())
    {
        std::uint8_t value{};

        // retrieve the item
        auto* item = items_.front();

        // send the opcode
        value = static_cast<std::uint8_t>(item->opcode);
        pClient_->send(&value, sizeof(value));

        // retrieve the value of the data
        if (item->szdata == 0) {
            std::uint16_t zero = 0;
            pClient_->send(reinterpret_cast<std::uint8_t*>(&zero), sizeof(zero));
        } else {
            pClient_->send(reinterpret_cast<std::uint8_t*>(&item->szdata), sizeof(item->szdata));
            pClient_->send(item->pdata, item->szdata);
        }

        // delete the item
        items_.pop();
        delete item;
    }

    // send the end of transmission
    pClient_->send(&Constants::Network::Protocol::eot, 1);
}

// receive data from the server
int KVClient::recv()
{
    VM::Opcodes_t op{VM::Opcodes_t::R_ERROR};

    // recreate the items
    std::uint8_t buffer[Constants::Network::Protocol::max_read_buffer] = {0};

    // read the Start-of-Transmission character
    int n = pClient_->recv(buffer, sizeof(std::uint8_t));

    if (buffer[0] != Constants::Network::Protocol::sot) {
        std::cerr << "Error: unable to find the SOT marker! [" << std::hex << buffer[0] << "]\n";
        // purge the socket
        while(n > 0) {
            n = pClient_->recv(buffer, Constants::Network::Protocol::max_read_buffer);
        }
        return -1;
    }

    // wait for the End-of-Transmission character
    while (true)
    {
        // read the next character
        n = pClient_->recv(buffer, sizeof(std::uint8_t));
        if ((buffer[0] == Constants::Network::Protocol::eot) || (n <= 0)) {
            break;
        }

        // this is an opcode
        op = static_cast<VM::Opcodes_t>(buffer[0]);

        // retrieve the size of the data
        pClient_->recv(buffer, sizeof(std::uint16_t));
        std::uint16_t* p = reinterpret_cast<std::uint16_t*>(buffer);

        int size{0};
        while (*p > 0) {

            // -1 to account for the '\0'
            if (*p > Constants::Network::Protocol::max_read_buffer) {
                size = Constants::Network::Protocol::max_read_buffer - 1;
            } else {
                size = *p;
            }

            // read the data and print it on screen
            memset(buffer, 0, Constants::Network::Protocol::max_read_buffer);
            n = pClient_->recv(buffer, size);
            std::cout << buffer;

            // decrease the initial size by the amount read
            *p = *p - n;
        }
    }

    // print the last line
    std::cout << std::endl;

    // return value according to received opcode from server
    if (op == VM::Opcodes_t::R_ERROR) {
        return -1;
    } else {
        return 0;
    }
}