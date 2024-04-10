/*
 * @file    interface.h
 * @brief   Network generic interface
 */

// ----- guards
#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

// ----- includes
#include <string>


// ----- class
namespace Network
{

    class Interface
    {
    public:
        Interface(std::string address, int port) :
            address_{address}, port_{port}
        { }

        virtual ~Interface()
        { }

        // no copy semantics
        Interface(const Interface&) = delete;
        Interface& operator=(const Interface&) = delete;

        // no move semantics
        Interface(Interface&&) = delete;
        Interface&& operator=(Interface&&) = delete;


    protected:
        std::string address_{};         //< the network address
        int port_{};                    //< the network port
        int socket_{};                  //< the network socket
    };

}

#endif // NETWORK_INTERFACE_H