/*
 * @file    cmdline.cpp
 * @brief   Source file for CmdLine class
 */

// ----- includes
#include "../constants.h"
#include "cmdline.h"


// ----- methods

namespace Application
{

// return a reference to args_ vector
Application::CmdLine::Args_t& CmdLine::args()
{
    return args_;
}

// return a reference to options_ vector
Application::CmdLine::Options_t& CmdLine::options()
{
    return options_;
}

// parse the command line and fill the args_ & options_ vectors
void CmdLine::parse(int argc, char* argv[])
{
    // initialize the command line vector
    cmdline_ = CmdLine_t(argv + 1, argv + argc);

    CmdLine_t::const_iterator it = cmdline_.begin();
    CmdLine_t::const_iterator tmp{nullptr};


    int options_count{0};

    // loop through the command line
    while (it != cmdline_.end())
    {
        // user configuration file
        if ((*it).compare("--config") == 0) {
            tmp = it;
            options_.push_back(*it);
            options_.push_back(
                ((it + 1) != cmdline_.end()) ? *(++it) : Constants::Config::filename
            );
            options_count += (it - tmp) + 1;
        }

        // database path
        if ((*it).compare("--database") == 0) {
            tmp = it;
            options_.push_back(*it);
            options_.push_back(
                ((it + 1) != cmdline_.end()) ? *(++it) : Constants::Config::database
            );
            options_count += (it - tmp) + 1;
        }

        // server mode
        if ((*it).compare("--serve") == 0) {
            options_.push_back(*it);
            options_count += 1;
        }

        // server address
        if ((*it).compare("--bind-address") == 0) {
            tmp = it;
            options_.push_back(*it);
            options_.push_back(
                ((it + 1) != cmdline_.end()) ? *(++it) : Constants::Config::srv_address
            );
            options_count += (it - tmp) + 1;
        }

        // server port
        if ((*it).compare("--bind-port") == 0) {
            tmp = it;
            options_.push_back(*it);
            options_.push_back(
                ((it + 1) != cmdline_.end()) ? *(++it) : Constants::Config::srv_port
            );
            options_count += (it - tmp) + 1;
        }

        // client address
        if ((*it).compare("--address") == 0) {
            tmp = it;
            options_.push_back(*it);
            options_.push_back(
                ((it + 1) != cmdline_.end()) ? *(++it) : Constants::Config::clt_address
            );
            options_count += (it - tmp) + 1;
        }

        // client port
        if ((*it).compare("--port") == 0) {
            tmp = it;
            options_.push_back(*it);
            options_.push_back(
                ((it + 1) != cmdline_.end()) ? *(++it) : Constants::Config::clt_port
            );
            options_count += (it - tmp) + 1;
        }

        // next item
        ++it;
    }

    // initialize the Arguments vector
    args_ = Args_t(cmdline_.begin() + options_count, cmdline_.end());
}


} //< end namespace

