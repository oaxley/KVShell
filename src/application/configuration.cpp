/*
 * @file    configuration.cpp
 * @brief   Source file for Configuration structure
 */

// ----- includes
#include "configuration.h"
#include "../constants.h"

#include <toml++/toml.hpp>

#include <unistd.h>

#include <filesystem>
#include <iostream>


// ----- member methods
namespace Application
{

// setup the configuration from the cmdline options
void Configuration::fromOptions(CmdLine::Options_t& options)
{
    CmdLine::Options_t::const_iterator it = options.begin();

    while (it != options.end())
    {
        // user configuration file
        if ((*it).compare("--config") == 0) {
            filename = *(++it);
        }

        // database
        if ((*it).compare("--database") == 0) {
            database = *(++it);
        }

        // server mode
        if ((*it).compare("--serve") == 0) {
            is_server = true;
        }

        // server address
        if ((*it).compare("--bind-address") == 0) {
            srv_address = *(++it);
        }

        // server port
        if ((*it).compare("--bind-port") == 0) {
            srv_port = *(++it);
        }

        // client address
        if ((*it).compare("--address") == 0) {
            clt_address = *(++it);
        }

        // client port
        if ((*it).compare("--port") == 0) {
            clt_port = *(++it);
        }

        // next item
        ++it;
    }
}

void Configuration::fromFile()
{
    // use the filename in the configuration
    if (filename.size() == 0)
        filename = Constants::Config::filename;

    // check if the file exists
    if (!std::filesystem::exists( std::filesystem::path{filename} )) {
        std::cerr << "Error: unable to find the configuration file [" << filename << "]\n";
        std:exit(EXIT_FAILURE);
    }

    // load the TOML file
    toml::table table;
    try {
        table = toml::parse_file(filename);
    } catch(const std::exception& e) {
        std::cerr << "Error: parsing failed\n";
        std::cerr << e.what() << '\n';
        std::exit(EXIT_FAILURE);
    }

    // merge the values
    // command line values have priority over configuration values
    using namespace std::string_view_literals;
    std::string_view value;

    // database
    value = table["database"]["path"].value_or(""sv);
    if ((value.size() != 0) && (database.size() == 0)) {
        database = value;
    }

    // server address
    value = table["server"]["address"].value_or(""sv);
    if ((value.size() != 0) && (srv_address.size() == 0)) {
        srv_address = value;
    }

    // server port : either a number or a string (representing a service)
    if (table["server"]["port"].is_integer())
    {
        int64_t number = static_cast<int64_t>(*table["server"]["port"].as_integer());
        if ((srv_port.size() == 0) && (number > 0)) {
            srv_port = std::to_string(number);
        }
    } else {
        value = table["server"]["port"].value_or(""sv);
        if ((value.size() != 0) && (srv_port.size() == 0)) {
            srv_port = value;
        }
    }

    // client address
    value = table["client"]["address"].value_or(""sv);
    if ((value.size() != 0) && (clt_address.size() == 0)) {
        clt_address = value;
    }

    // client port : either a number or a string (representing a service)
    if (table["client"]["port"].is_integer())
    {
        int64_t number = static_cast<int64_t>(*table["client"]["port"].as_integer());
        if ((clt_port.size() == 0) && (number > 0)) {
            clt_port = std::to_string(number);
        }
    } else {
        value = table["client"]["port"].value_or(""sv);
        if ((value.size() != 0) && (clt_port.size() == 0)) {
            clt_port = value;
        }
    }
}

void Configuration::finalize()
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

void Configuration::dump()
{
#ifdef DEBUG
    std::cerr << "----- Program -----\n";
    std::cerr << "name    : " << Constants::program_name << "\n";
    std::cerr << "version : " << Constants::program_version << "\n";
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

} //< end namespace
