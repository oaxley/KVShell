// main entry point

#include "application.h"
#include "constants.h"

#include <iostream>

int main(int argc, char* argv[])
{
    // print program name & version
    std::cout << Constants::program_name << " - v" << Constants::program_version << "\n";

    // retrieve application instance
    Application::Instance& app = Application::Instance::get();

    // parse the command line parameters
    Application::Args& args = app.args();
    args.parse(argc, argv);

    // retrieve configuration element
    Application::Configuration& config = app.config();
    config.loadFromArgs(args);
    config.loadFromFile();
    config.finalize();
    config.dump();


    return EXIT_SUCCESS;
}