#include "../include/raii_wrappers.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>

Socket::~Socket() {
    if (fd != INVALID_SOCKET) {
        shutdown(fd, SD_BOTH);
        closesocket(fd);
    }

    if (sockaddr_p)
        delete [] sockaddr_p;
};

Socket::Socket(Socket&& other) {
    if(this != &other) {
        fd = INVALID_SOCKET;
        std::swap(fd, other.fd);
        
        sockaddr_p = nullptr;
        std::swap(sockaddr_p, other.sockaddr_p);
    }
}

Socket& Socket::operator=(Socket&& other) {
    if(this != &other) {
        if(fd != INVALID_SOCKET) {
            shutdown(fd, SD_BOTH);
            closesocket(fd);
        }
        fd = INVALID_SOCKET;
        std::swap(fd, other.fd);

        if(sockaddr_p) {
            delete [] sockaddr_p;
            sockaddr_p = nullptr;
        }
        
        std::swap(sockaddr_p, other.sockaddr_p);
    }
    return *this;
}

std::ostream& operator<<(std::ostream& out, Socket& sock) {
    if (sock.sockaddr_p) {
        struct sockaddr * result = sock.sockaddr_p;
        void *addr;
        std::string ipver;
        struct sockaddr_in *ipv4;
        struct sockaddr_in6 *ipv6;
        unsigned short port;
        char ipstr[INET6_ADDRSTRLEN];
        
        if ( result->sa_family == AF_INET )
        {
            ipv4 = reinterpret_cast<struct sockaddr_in *>(result);
            addr = &(ipv4->sin_addr);
            port = ntohs(ipv4 -> sin_port);
            ipver = "IPv4";
        }
        else if ( result->sa_family == AF_INET6 )
        {
            ipv6 = reinterpret_cast<struct sockaddr_in6 *>(result);
            addr = &(ipv6->sin6_addr);
            port = ntohs(ipv6 -> sin6_port);
            ipver = "IPv6";
        }

        if ( inet_ntop(result->sa_family, addr, ipstr, sizeof(ipstr)) == 0)
        {
            out << "";
            return out;
        }
        out << "{Type: " << ipver << ", IP: " << ipstr << ", PORT: " << port << "}";
    
    } else {
        out << "";
    }
    return out;
}