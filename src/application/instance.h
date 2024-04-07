/*
 * @file    instance.h
 * @brief   Header file for Instance class
 */

// ----- guards
#ifndef INSTANCE_H
#define INSTANCE_H

// ----- includes
#include "args.h"

#include <mutex>


// ----- class
namespace Application
{

    class Instance final
    {
    public:     //< public methods
        // return the current instance of the class
        static Instance& get();

        // no copy semantics
        Instance(const Instance&) = delete;
        Instance& operator=(const Instance&) = delete;

        Args& args();

    private:    //< private methods
        Instance() = default;
        ~Instance() = default;

    private:    //< private members
        static std::mutex mutex_;
        Args args_;
    };

}

#endif // INSTANCE_H
