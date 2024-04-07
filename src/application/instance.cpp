/*
 * @file    instance.cpp
 * @brief   Source file for Instance class
 */

// ----- includes
#include "instance.h"

// ----- members definition
std::mutex Application::Instance::mutex_;


// ----- methods definition

// return the current instance of the application
/*static*/ Application::Instance& Application::Instance::get()
{
    std::lock_guard<std::mutex> lock(mutex_);

    // create the class instance during the very first call
    static Instance inst;

    return inst;
}

// return the instance of the argument parser
Application::Args& Application::Instance::args()
{
    return args_;
}

// return the instance of the configuration
Application::Configuration& Application::Instance::config()
{
    return config_;
}