/*
 * @file    helpers.cpp
 * @brief   Source file for helper functions
 */

// ----- includes
#include "helpers.h"

#include <string.h>

#include <iostream>


namespace VM
{

// retrieve the UID from the U_USER block
int getUID(QueueItem* item)
{
    if (item->opcode != Opcodes_t::U_USER) {
        std::cerr << "Error: block is not a U_USER in VM::getUID!\n";
        return -1;
    }

    // cast the pointer to an int
    int* p = reinterpret_cast<int*>(item->pdata);
    return *p;
}

// retrieve the key from the K_NAME block
std::uint8_t* getKey(QueueItem* item, std::uint16_t* size)
{
    if (item->opcode != Opcodes_t::K_NAME) {
        std::cerr << "Error: block is not a K_NAME in VM::getKey!\n";
        return nullptr;
    }

    // return the size to the caller
    *size = item->szdata;

    // copy the data to a different block in memory
    std::uint8_t* dest = new std::uint8_t[item->szdata + 1];
    memset(dest, 0, item->szdata + 1);
    memcpy(dest, item->pdata, item->szdata);

    return dest;
}

// retrieve the value from the V_STR or V_STDIN block
std::uint8_t* getValue(QueueItem* item)
{
    if ((item->opcode != Opcodes_t::V_STR) && (item->opcode != Opcodes_t::V_STDIN)) {
        std::cerr << "Error: block is not a V_STR or V_STDIN in VM::getValue!\n";
        return nullptr;
    }

    return nullptr;
}


} //< end of namespace
