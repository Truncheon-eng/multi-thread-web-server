#include <thread>
#include <cstring>
#include <cstdio>
#include <functional>
#include <map>
#include <mutex>
#include <iostream>

#include "../include/raii_wrappers.hpp"
#include "../include/lab_macroses.hpp"
#include "../include/operations.hpp"
#include "../include/errors.hpp"

using std::unique_ptr;
using std::make_unique;
using std::string;

std::mutex& get_mutex(void) {
    static std::mutex mtx;
    return mtx;
}


int main(int argc, char *argv[]) {
    int iResult;

    if (argc != 2) {
        ErrorOutputWindows("argc != 2", argc, LAB_FLAG_MY);
        return ERR;
    }
    
    WinsockSession session; // wrapper of WSAStartup and WSAClenup
    
    SAddrInfo addr_p {GetAddrInfo(argv[1])};
    
    if (addr_p.get() == nullptr) {
        ErrorOutputWindows("LabGetAddrInfo failed with error", ERR, LAB_FLAG_WSA);
        return ERR;
    }
    
    Socket ServerSocket = InitServerSocket(addr_p);

    if (ServerSocket.get() == INVALID_SOCKET) {
        ErrorOutputWindows("InitServerSocket error", ERR, LAB_FLAG_WSA);
        return ERR;
    }

    std::cout << "[Server sockaddr]: " << ServerSocket << std::endl;

    std::map<SOCKET, Socket> clients;

    std::function<void(Socket&)> ClientHandler = [](Socket& sock) -> void {
        SOCKET client = sock.get();
        
        std::string client_req;

        int read = 0;

        do {
            char buf[256];
            std::memset(buf, '\0', sizeof(buf));
            read = recv(sock.get(), buf, sizeof(buf) - 1, 0);
            client_req += std::string(buf);
        } while( read > 0 );

        send(client, client_req.c_str(), client_req.size(), 0);

        {
            std::lock_guard<std::mutex> lck_guard(get_mutex());
            std::cout << "{" << std::endl;
            std::cout << "\t[Client sockaddr]: " << sock << std::endl;
            std::cout << "\t[Client message]: " << client_req;
            std::cout << "}" << std::endl;
        }

        return;
    };

    while(true) {
        iResult = listen(ServerSocket.get(), SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            ErrorOutputWindows("listen error", ERR, LAB_FLAG_WSA);
            return ERR;
        }

        struct sockaddr addr_;

        socklen_t len = sizeof(addr_);

        SOCKET raw_client_socket = accept(ServerSocket.get(), &addr_, &len); 

        Socket client {raw_client_socket, &addr_};
        
        if(client.get() == INVALID_SOCKET && WSAGetLastError() != WSAEWOULDBLOCK) {
            ErrorOutputWindows("accept error", ERR, LAB_FLAG_WSA);
            return ERR;
        }
    
        clients[raw_client_socket] = std::move(client);

        std::vector<WSAPOLLFD> poll;

        for(auto & p: clients) {
            WSAPOLLFD pollfd;
            pollfd.fd = p.first;
            pollfd.events = POLLIN;
            pollfd.revents = 0;
            poll.push_back(pollfd);
        }

        WSAPoll(poll.data(), poll.size(), 0);

        std::vector<std::thread> handlers;

        for(auto& ws: poll) {
            
            if (ws.revents & POLLIN) {
                {
                    std::lock_guard<std::mutex> lck_guard{get_mutex()};
                    std::cout << "[DEBUG]: " << "POLLIN - " << clients[ws.fd] << std::endl;
                }

                handlers.emplace_back(ClientHandler, std::ref(clients[ws.fd]));
                
                continue;
            }

            if (ws.revents & (POLLHUP | POLLERR)) {
                {
                    std::lock_guard<std::mutex> lck_guard{get_mutex()};
                    std::cout << "[DEBUG]: " << "POLLHUP | POLLERR - " << clients[ws.fd] << std::endl;
                    clients.erase(ws.fd);
                }
            }

        }

        for (auto& thread: handlers) // joining of threads
            thread.join();

    }

    return 0;
}