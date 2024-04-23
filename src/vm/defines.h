/*
 * @file    defines.h
 * @brief   Header file containing definitions for the Virtual Machine
 */

// ----- guards
#ifndef DEFINES_H
#define DEFINES_H


// ----- includes
#include <cstdint>
#include <queue>


// ----- definitions

namespace VM
{

// ----- enums
enum class Opcodes_t {
    // ----- OPERATORS
    OP_SET,                //< "SET KEY VALUE" | "SET KEY < something" | "something | SET KEY"

    OP_GET,                //< "GET KEY"

    OP_EXPDT,              //< "EXPIRE KEY DATETIME"
    OP_EXPDR,              //< "EXPIRE KEY DURATION"

    OP_DEL,                //< "DELETE KEY"

    OP_PRT,                //< "PRINT 'regexp'"

    OP_EXIST,              //< "EXIST KEY"

    // ----- KEY
    K_NAME,                 //< Standard string for key

    // ----- VALUES
    V_STR,                  //< String Value
    V_STDIN,                //< Read STDIN for input
    V_DT,                   //< Date Time value
    V_DUR,                  //< Duration with unit
    V_REGEXP,               //< Regular Expression

    // ----- RESP
    R_VALUE,                //< Response from Server
    R_ERROR,                //< Error from the Server
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