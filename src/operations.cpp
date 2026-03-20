#include "../include/raii_wrappers.hpp"
#include "../include/lab_macroses.hpp"

#include <functional>
#include <iostream>
#include <ws2tcpip.h>
#include <memory>
#include <cstring>
#include <sstream>

using SAddrInfo = std::shared_ptr<struct addrinfo>;

std::function<void(struct addrinfo *)> AddrInfoDeleter = [](struct addrinfo * addr_p) {
    if(addr_p != nullptr)
        freeaddrinfo(addr_p);
};

SAddrInfo GetAddrInfo(const char* port) {
    struct addrinfo* addr_p = nullptr;
    struct addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(nullptr, port, &hints, &addr_p) != 0) {
        return SAddrInfo(nullptr, AddrInfoDeleter);
    }

    return SAddrInfo(addr_p, AddrInfoDeleter);
}

// полная копия LabPrintSockAddr из лабораторнойработы
// std::string PrintSockAddr(const struct sockaddr *result)
// {
//     std::stringstream ss;



//     return ss.str();
// }

// полная копия прохода по односвязному списку
// int PrintAddrInfo(struct addrinfo *result)
// {
//     struct addrinfo *pointer;
//     int iResult = 0;
    
//     for ( pointer = result; pointer != NULL; pointer = pointer->ai_next )
//     {
//         iResult = PrintSockAddr(pointer->ai_addr);
//     }
    
//     return iResult;
// }

// создание серверного сокета, binding и маркировка на неблокирующие операции
Socket InitServerSocket(SAddrInfo addr_p) {
    SOCKET sock = socket(addr_p -> ai_family, addr_p -> ai_socktype, addr_p -> ai_protocol);
    
    if (sock == INVALID_SOCKET)
        return Socket(sock);

    if (bind(sock, addr_p -> ai_addr, addr_p -> ai_addrlen) != 0) {
        closesocket(sock);
        return Socket(INVALID_SOCKET);
    }

    u_long iMode = 1;
    if(ioctlsocket(sock, FIONBIO, &iMode) != 0) {
        closesocket(sock);
        return Socket(INVALID_SOCKET);
    }

    return Socket(sock, addr_p -> ai_addr);
}