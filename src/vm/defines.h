/*
 * @file    defines.h
 * @brief   Header file containing definitions for the Virtual Machine
 */

// ----- guards
#ifndef DEFINES_H
#define DEFINES_H


// ----- includes
#include <cstdint>


// ----- definitions

namespace VM
{

// ----- enums
enum class Opcodes_t {
    // ----- OPERATORS
    OP_SET1,                //< "SET KEY < something" or "something | SET KEY"
    OP_SET2,                //< "SET KEY VALUE"

    OP_GET1,                //< "GET KEY"

    OP_EXPDT2,              //< "EXPIRE KEY DATETIME"
    OP_EXPDR2,              //< "EXPIRE KEY DURATION"

    OP_DEL1,                //< "DELETE KEY"

    OP_PRT1,                //< "PRINT 'regexp'"

    OP_EXIST1,              //< "EXIST KEY"

    // ----- KEY
    K_NAME,                 //< Standard string for key

    // ----- VALUES
    V_STR,                  //< String Value
    V_STDIN,                //< Read STDIN for input
    V_DT,                   //< Date Time value
    V_DUR,                  //< Duration with unit
    V_REGEXP,               //< Regular Expression
};

// queue item
struct QueueItem
{
    Opcodes_t opcode;               //< the opcode
    std::uint16_t szdata;           //< size of the following data (max 64KiB)
    std::uint8_t* pdata;            //< pointer to the data

    ~QueueItem() {
        delete [] pdata;
    }
};

// typedef
using queue_t = std::queue<QueueItem*>;

} //< end of namespace

#endif // DEFINES_H