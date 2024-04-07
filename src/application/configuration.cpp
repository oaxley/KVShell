/*
 * @file    configuration.cpp
 * @brief   Source file for Configuration structure
 */

// ----- includes
#include "configuration.h"
#include "../constants.h"

#include <unistd.h>

#include <iostream>


// ----- member methods
// initialize the configuration from the command line arguments
void Application::Configuration::loadFromArgs(Application::Args& args)
{
    Application::Args::Iterator it = args.begin();
    while (it != args.end())
    {
        // user configuration file
        if ((*it).compare("--config") == 0) {
            filename = ((it + 1) != args.end()) ? *(++it) : Constants::Config::filename;
        }

        // database path
        if ((*it).compare("--database") == 0) {
            database = ((it + 1) != args.end()) ? *(++it) : Constants::Config::database;
        }

        // server mode
        if ((*it).compare("--serve") == 0) {
            is_server = true;
        }

        if ((*it).compare("--bind-address") == 0) {
            srv_address = ((it + 1) != args.end()) ? *(++it) : Constants::Config::srv_address;
        }

        if ((*it).compare("--bind-port") == 0) {
            srv_port = ((it + 1) != args.end()) ? *(++it) : Constants::Config::srv_port;
        }

        if ((*it).compare("--address") == 0) {
            clt_address = ((it + 1) != args.end()) ? *(++it) : Constants::Config::clt_address;
        }

        if ((*it).compare("--port") == 0) {
            clt_port = ((it + 1) != args.end()) ? *(++it) : Constants::Config::clt_port;
        }

        // next item
        ++it;
    }
}

// finalize the configuration
void Application::Configuration::finalize()
{
    // retrieve the UID/GID from the user
    uid = getuid();
    gid = getgid();

    // fill all the blanks :)
    if (filename.size() == 0)
        filename = Constants::Config::filename;

    if (database.size() == 0)
        database = Constants::Config::database;

    if (srv_address.size() == 0)
        srv_address = Constants::Config::srv_address;

    if (srv_port.size() == 0)
        srv_port = Constants::Config::srv_port;

    if (clt_address.size() == 0)
        clt_address = Constants::Config::clt_address;

    if (clt_port.size() == 0)
        clt_port = Constants::Config::clt_port;
}

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

