#include <not_implemented.h>
// #include <httplib.h>
#include "../include/server_logger.h"

#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

server_logger::~server_logger() noexcept
{
    int header[] = {2, inner_getpid(), 0, 0};

    std::string full_msg(reinterpret_cast<const char*>(header), 16);
    send_to_server(_socket_fd, full_msg);

#ifdef _WIN32
    closesocket(_socket_fd);
    WSACleanup();
#else
    close(_socket_fd);
#endif
}

logger& server_logger::log(
    const std::string &text,
    logger::severity severity) &
{
    int header[] = {1, inner_getpid(), static_cast<int>(severity), static_cast<int>(text.size())};

    std::string full_msg(reinterpret_cast<const char*>(header), 16);
    full_msg += text;

    //std::cout << "log send (" << text << ")" << std::endl;

    send_to_server(_socket_fd, full_msg);
    return *this;
}

server_logger::server_logger(const std::string& dest,
                             const std::unordered_map<logger::severity, std::pair<std::string, bool>> &streams)
{
    std::stringstream body;
    for (const auto& [sev, pair] : streams) {
        body << static_cast<int>(sev) << ' ' << pair.first << ' ' << (pair.second ? "1" : "0") << '\n';
    }
    std::string body_str = body.str();

    int header[] = {0, inner_getpid(), 0, static_cast<int>(body_str.size())}; // type, pid, severity, size
    std::string full_msg(reinterpret_cast<const char*>(header), 16);
    full_msg += body_str;

    std::string host = "127.0.0.1";
    int port = 9200;
    std::string netloc = dest;
    if (netloc.rfind("http://", 0) == 0) {
        netloc = netloc.substr(7);
    }

    auto pos = netloc.find(':');
    if (pos != std::string::npos) {
        host = netloc.substr(0, pos);
        port = std::stoi(netloc.substr(pos + 1));
    }
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket_fd < 0) return;

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr);

    if (connect(_socket_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
#ifdef _WIN32
        closesocket(_socket_fd);
        WSACleanup();
#else
        close(_socket_fd);
#endif
        return;
    }

    send_to_server(_socket_fd, full_msg);
}

void server_logger::send_to_server(const int& socket_fd, const std::string& message)
{
    if (socket_fd > 0) {
        send(socket_fd, message.c_str(), message.size(), 0);
    }
}

int server_logger::inner_getpid()
{
#ifdef _WIN32
    return ::_getpid();
#else
    return getpid();
#endif
}

server_logger::server_logger(const server_logger &other)  = default;

server_logger &server_logger::operator=(const server_logger &other) = default;

server_logger::server_logger(server_logger &&other) noexcept = default;

server_logger &server_logger::operator=(server_logger &&other) noexcept = default;
