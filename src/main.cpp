// main entry point

#include "application.h"
#include "constants.h"
#include "kvserver.h"
#include "network.h"

#include <iostream>


int main(int argc, char* argv[])
{
    // print program name & version
    std::cout << Constants::program_name << " - v" << Constants::program_version << "\n";

    // setup application instance
    Application::Instance& app = Application::Instance::get();
    app.parse(argc, argv);
    app.loadConfig();

    // start the TCP Server
    if (app.config().is_server) {
        KVServer kvserver(app.config().srv_address, app.config().srv_port, app.config().database);
        kvserver.start();
    }



    return EXIT_SUCCESS;
}