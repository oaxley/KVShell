/*
 * @file    configuration.h
 * @brief   Header file for Configuration structure
 */

// ----- guards
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// ----- includes
#include "cmdline.h"

#include <string>


// ----- structure
namespace Application
{

    struct Configuration
    {
        // ----- members
        std::string filename{};         //< TOML configuration file path
        std::string database{};         //< SQLite database path

        bool is_server{false};          //< true if the application is running in server mode (client otherwise)
        std::string srv_address{};      //< the binding interface address (default: 0.0.0.0)
        std::string srv_port{};         //< the binding port (default: 4567)

        std::string clt_address{};      //< the TCP address for the client connection (default: localhost)
        std::string clt_port{};         //< the TCP port for the client connection (default: 4567)

        int uid{};                      //< Unix user ID
        int gid{};                      //< Unix group ID

        // ----- methods
        void fromOptions(Application::CmdLine::Options_t& options);
        void fromFile();
        void finalize();
        void dump();
    };

}

#endif // CONFIGURATION_H
