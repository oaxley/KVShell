/*
 * @file    interface.h
 * @brief   Source file for Interface class
 */

// ----- includes
#include "interface.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>

namespace Network
{

// ----- member methods
// resolve a service name to its TCP port number
int Interface::resolvePort(std::string port)
{
    // try to directly convert the string into a number
    try
    {
        int value = std::stoi(port);
        return value;
    }
    catch(const std::exception& e)
    {
        // do nothing as it might be a service name
    }

    // lookup in /etc/services to deduct the TCP port number
    struct servent* entry{nullptr};
    entry = getservbyname(port.c_str(), "tcp");

    if (entry)
        return htons(entry->s_port);

    return -1;
}

// resolve an address to its IP
void* Interface::resolveAddr(std::string address)
{
    struct in_addr addr;
    struct hostent* host;

    // look for an IP first
    if (inet_aton(address.c_str(), &addr) != 0) {
        host = gethostbyaddr((char*)&addr, sizeof(struct in_addr), AF_INET);
        if (host == nullptr) {
            return nullptr;
        }
    } else {
        // name resolution
        host = gethostbyname(address.c_str());
        if (host == nullptr) {
            return nullptr;
        }
    }

    // retrieve the IP
    struct in_addr* ip = (struct in_addr*)(host->h_addr_list[0]);
    return static_cast<void*>(ip);
}

} //< end namespace