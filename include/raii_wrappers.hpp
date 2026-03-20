#pragma once

#include "lab_macroses.hpp"
#include <winsock2.h>
#include <cstring>
#include <iostream>

class Socket {
    SOCKET fd;
    struct sockaddr * sockaddr_p;
public:
    
    explicit Socket(SOCKET sock = INVALID_SOCKET, struct sockaddr * ptr = nullptr): fd{sock} {
        sockaddr_p = reinterpret_cast<struct sockaddr *>(
            new char[sizeof(struct sockaddr)]
        );
        
        if(ptr)
            std::memmove(sockaddr_p, ptr, sizeof(struct sockaddr));
        else
            std::memset(sockaddr_p, 0, sizeof(struct sockaddr));
    };

    Socket(Socket&& other);

    Socket& operator=(Socket&& other);

    ~Socket();

    SOCKET get() const {
        return fd;
    };

    friend std::ostream& operator<<(std::ostream& out, Socket& sock);

private:
    Socket(Socket&) = delete;
    Socket(const Socket&) = delete;
    void operator=(const Socket&) = delete;
    void operator=(Socket&) = delete;
};


class WinsockSession {
public:
    WinsockSession(BYTE minor = 2, BYTE major = 2);
    ~WinsockSession();
private:
    WinsockSession(WinsockSession&) = delete;
    WinsockSession(const WinsockSession&) = delete;

    void operator=(WinsockSession&) = delete;
    void operator=(const WinsockSession&) = delete;
};

std::ostream& operator<<(std::ostream&, const Socket&);