#include "../include/raii_wrappers.hpp"

#include <stdexcept>
#include <string>
#include <iomanip>

WinsockSession::WinsockSession(BYTE minor, BYTE major) {
    WSADATA data;
    int result = WSAStartup(MAKEWORD(major, minor), &data);
    if (result != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(result));
    }
}

WinsockSession::~WinsockSession() {
    WSACleanup();
}