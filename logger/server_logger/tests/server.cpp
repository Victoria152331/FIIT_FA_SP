//
// Created by Des Caldnd on 3/27/2024.
//

#include "server.h"
#include <logger_builder.h>
#include <fstream>
#include <iostream>


#include <thread>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>

server::server(uint16_t port)
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed\n";
        close(listen_fd);
        return;
    }

    if (listen(listen_fd, SOMAXCONN) < 0) {
        std::cerr << "Listen failed\n";
        close(listen_fd);
        return;
    }

    std::cout << "Server is listening on port " << port << "\n";

    while (true) {
        int client_fd = accept(listen_fd, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::thread([this, client_fd]() {
            handle_client(client_fd);
        }).detach();
    }
}

void server::handle_client(int client_fd)
{
    char header[16];
    while (true) {
        //std::cout << "wait..." << client_fd << std::endl;
        int received = recv(client_fd, header, 16, MSG_WAITALL);
        //std::cout << "O!" << client_fd  << std::endl;
        if (received != 16) break;
        //std::cout << "O2!" << client_fd  << std::endl;

        int* fields = reinterpret_cast<int*>(header);
        int type = fields[0];
        int pid = fields[1];
        int severity = fields[2];
        int length = fields[3];

        std::string body;
        body.resize(length);

        int recv_length = recv(client_fd, body.data(), length, MSG_WAITALL);
        if (recv_length != length) break;


        if (type == 0) { // new
            //std::cout << "new" << std::endl;

            std::istringstream iss(body);
            std::unordered_map<logger::severity, std::pair<std::string, bool>> client_streams;
            std::string line;
            while (std::getline(iss, line)) {
                std::istringstream line_stream(line);
                int sev;
                std::string file;
                int cons;
                line_stream >> sev >> file >> cons;
                client_streams[static_cast<logger::severity>(sev)] = {file, cons == 1};
                //std::cout << sev << " " << file << " " << cons << std::endl;
            }

            
            std::unique_lock<std::shared_mutex> lock(_mut);
            _streams[pid] = client_streams;
            
            //std::cout << "end new" << std::endl;
        } else if (type == 1) { //log
            //std::cout << "log" << std::endl;
            //std::cout << "(" << body << ")" << std::endl;

            std::shared_lock<std::shared_mutex> lock(_mut);
            auto pid_it = _streams.find(pid);
            if (pid_it != _streams.end()) {
                auto& map = pid_it->second;
                auto it = map.find(static_cast<logger::severity>(severity));
                if (it != map.end()) {
                    const auto& [file_path, to_console] = it->second;
                    if (file_path != "0") {
                        std::ofstream out(file_path, std::ios::app);
                        if (out.is_open())
                            out << body << std::endl;
                    }
                    if (to_console) {
                        std::cout << body << std::endl;
                    }
                }
            }
            //std::cout << "end log" << std::endl;
        } else if (type == 2) { //del
            //std::cout << "del" << std::endl;
            std::unique_lock<std::shared_mutex> lock(_mut);
            _streams.erase(pid);
            //std::cout << "end del" << std::endl;
            break;
        }
    }
    
    close(client_fd);
}
