// main entry point

#include "application.h"
#include "constants.h"

#include <iostream>

int main(int argc, char* argv[])
{
    // print program name & version
    std::cout << Constants::program_name << " - v" << Constants::program_version << "\n";

    // setup application instance
    Application::Instance& app = Application::Instance::get();
    app.parse(argc, argv);
    app.loadConfig();


    return EXIT_SUCCESS;
}