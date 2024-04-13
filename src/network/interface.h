/*
 * @file    interface.h
 * @brief   Header file for Interface class
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
    public:     //< public methods

        Interface(std::string address, std::string port) :
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

        // resolvers
        static int resolvePort(std::string port);
        static void* resolveAddr(std::string address);

    protected:  //< private members

        std::string address_{};         //< the network address
        std::string port_{};            //< the network port
        int socket_{};                  //< the network socket
    };

}

#endif // NETWORK_INTERFACE_H