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
KVClient::KVClient()
{
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
            VM::QueueItem* item = new VM::QueueItem;

            item->opcode = (args_size == 2) ? VM::Opcodes_t::OP_SET1 : VM::Opcodes_t::OP_SET2;
            item->szdata = 0;
            item->pdata = nullptr;

            items_.push(item);
            ++it;

            // read the Key Name
            getKeyName(*(it++));

            // read the value
            if (args_size == 2) {
                getValue("");
            } else {
                getValue(*(it++));
            }
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
        if (remaining > Constants::KVClient::max_item_size) {
            block_size = Constants::KVClient::max_item_size;
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
        // look up for the first character if it's a file
        if (arg.at(0) == '@') {
            itemFromArg(arg, VM::Opcodes_t::V_FILE);
        } else {
            itemFromArg(arg, VM::Opcodes_t::V_STR);
        }

        return;
    }

    // data are passed from STDIN either with "<" or a pipe "|"
    if (!isatty(fileno(stdin))) {
        std::uint8_t buffer[Constants::KVClient::max_item_size];
        while (true)
        {
            int n = read(fileno(stdin), buffer, Constants::KVClient::max_item_size);

            // nothing to read anymore
            if (n <= 0)
                break;

            // create a new block
            VM::QueueItem* item = new VM::QueueItem {
                opcode: VM::Opcodes_t::V_STDIN,
                szdata: static_cast<std::uint16_t>(n),
                pdata: new std::uint8_t[n]
            };

            // copy the data to the block
            memcpy(item->pdata, buffer, n);

            items_.push(item);
        }
    }
}