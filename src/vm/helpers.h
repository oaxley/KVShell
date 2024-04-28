/*
 * @file    helpers.h
 * @brief   Header file for helper functions
 */

#ifndef VM_HELPERS_H
#define VM_HELPERS_H

// ----- include
#include "defines.h"

#include <cstdint>


namespace VM
{

// ----- functions

// retrieve the UID from the U_USER block
int getUID(QueueItem* item);

// retrieve the data from a block
std::uint8_t* getData(QueueItem* item, std::uint16_t* size);

// // retrieve the key from the K_NAME block
// std::uint8_t* getKey(QueueItem* item, std::uint16_t* size);

// // retrieve the value from the V_STR or V_STDIN block
// std::uint8_t* getValue(QueueItem* item);


} //< end of namespace

#endif // VM_HELPERS_H