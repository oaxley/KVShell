// main entry point

#include "application.h"
#include "kvclient.h"
#include "kvserver.h"


int main(int argc, char* argv[])
{
    int retval = EXIT_SUCCESS;

    // setup application instance
    Application::Instance& app = Application::Instance::get();
    app.parse(argc, argv);
    app.loadConfig();

    // check if user wants help
    if (app.config().is_help) {
        app.config().printHelp();
        std::exit(EXIT_SUCCESS);
    }

    // start the TCP Server
    if (app.config().is_server) {
        KVServer kvserver(app.config().srv_address, app.config().srv_port, app.config().database);
        kvserver.start();
    } else {
        // create a new client instance
        KVClient kvclient(app.config().clt_address, app.config().clt_port);
        kvclient.setUser(app.config().uid, app.config().gid);

        // parse the command line
        kvclient.parse(app.cmdline());

        // send the data to the server
        kvclient.send();

        // read the server's response
        retval = kvclient.recv();
    }

    return retval;
}