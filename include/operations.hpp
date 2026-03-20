#pragma once

#include <memory>
#include <ws2tcpip.h>
#include <winsock2.h>
#include "raii_wrappers.hpp"

using SAddrInfo = std::shared_ptr<struct addrinfo>;

SAddrInfo GetAddrInfo(const char* port);
Socket InitServerSocket(SAddrInfo addr_p);