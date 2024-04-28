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

// retrieve the data from a block
std::uint8_t* getData(QueueItem* item, std::uint16_t* size)
{
    // return the size to the caller
    *size = item->szdata;

    // copy the data to a different block in memory
    std::uint8_t* dest = new std::uint8_t[item->szdata + 1];
    memset(dest, 0, item->szdata + 1);
    memcpy(dest, item->pdata, item->szdata);

    return dest;
}

} //< end of namespace
