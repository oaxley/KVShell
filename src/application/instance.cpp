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

// return the instance of the configuration
Application::Configuration& Application::Instance::config()
{
    return config_;
}

// parse the command line
void Application::Instance::parse(int argc, char* argv[])
{
    cmdline_.parse(argc, argv);
}

// return a reference to the cmdline
Application::CmdLine& Application::Instance::cmdline()
{
    return cmdline_;
}

// load the configuration from the options and the file
void Application::Instance::loadConfig()
{
    // load from the command line options
    config_.fromOptions(cmdline_.options());

    // load from the configuration file
    config_.fromFile();

    // finalize the configuration
    config_.finalize();

    // dump the configuration in debug mode
    config_.dump();
}