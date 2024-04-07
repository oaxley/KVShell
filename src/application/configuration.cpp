/*
 * @file    configuration.cpp
 * @brief   Source file for Configuration structure
 */

// ----- includes
#include "configuration.h"

#include <iostream>


// ----- member methods
// dump the configuration (debug only)
void Application::Configuration::dump()
{
#ifdef DEBUG
    std::cerr << "----- Configuration -----\n";
    std::cerr << "filename    : " << filename << "\n";
    std::cerr << "database    : " << database << "\n";
    std::cerr << "is_server   : " << std::boolalpha << is_server << "\n";
    std::cerr << "srv_address : " << srv_address << "\n";
    std::cerr << "srv_port    : " << srv_port << "\n";
    std::cerr << "clt_address : " << clt_address << "\n";
    std::cerr << "clt_port    : " << clt_port << "\n";
    std::cerr << "uid         : " << uid << "\n";
    std::cerr << "gid         : " << gid << "\n";
#endif
}

