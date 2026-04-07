#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

static void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " <host> [start_port] [end_port] [timeout_ms]\n"
              << "Example: " << progName << " 192.168.1.10 1 1024 200\n";
}

#ifdef _WIN32
using SocketType = SOCKET;
const SocketType InvalidSocket = INVALID_SOCKET;
#else
using SocketType = int;
const SocketType InvalidSocket = -1;
#endif

static void closeSocket(SocketType sock) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

static bool setNonBlocking(SocketType sock) {
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(sock, FIONBIO, &mode) == 0;
#else
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0) return false;
    return fcntl(sock, F_SETFL, flags | O_NONBLOCK) == 0;
#endif
}

static bool waitForConnect(SocketType sock, int timeoutMs) {
    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(sock, &writeSet);

    fd_set errorSet;
    FD_ZERO(&errorSet);
    FD_SET(sock, &errorSet);

    timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    int result = select(static_cast<int>(sock + 1), nullptr, &writeSet, &errorSet, &tv);
    if (result <= 0) return false;

    if (FD_ISSET(sock, &errorSet)) return false;
    if (!FD_ISSET(sock, &writeSet)) return false;

    int opt = 0;
    socklen_t optLen = sizeof(opt);
#ifdef _WIN32
    if (getsockopt(sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&opt), &optLen) != 0)
        return false;
#else
    if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &opt, &optLen) != 0)
        return false;
#endif
    return opt == 0;
}

static bool scanPort(const std::string& host, int port, int timeoutMs) {
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string portStr = std::to_string(port);
    struct addrinfo* result = nullptr;
    if (getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result) != 0) {
        return false;
    }

    bool open = false;
    for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        SocketType sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sock == InvalidSocket) continue;

        if (!setNonBlocking(sock)) {
            closeSocket(sock);
            continue;
        }

        int connectResult = connect(sock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
#ifdef _WIN32
        if (connectResult == 0) {
            open = true;
        } else {
            int lastError = WSAGetLastError();
            if (lastError == WSAEWOULDBLOCK || lastError == WSAEINPROGRESS) {
                open = waitForConnect(sock, timeoutMs);
            }
        }
#else
        if (connectResult == 0) {
            open = true;
        } else if (errno == EINPROGRESS) {
            open = waitForConnect(sock, timeoutMs);
        }
#endif
        closeSocket(sock);
        if (open) break;
    }

    freeaddrinfo(result);
    return open;
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
#endif

    if (argc < 2) {
        printUsage(argv[0]);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    std::string host = argv[1];
    int startPort = 1;
    int endPort = 1024;
    int timeoutMs = 200;

    if (argc >= 3) startPort = std::atoi(argv[2]);
    if (argc >= 4) endPort = std::atoi(argv[3]);
    if (argc >= 5) timeoutMs = std::atoi(argv[4]);

    if (startPort < 1) startPort = 1;
    if (endPort > 65535) endPort = 65535;
    if (endPort < startPort) endPort = startPort;
    if (timeoutMs < 10) timeoutMs = 10;

    std::cout << "Scan de ports sur " << host << " de " << startPort << " à " << endPort
              << " (timeout=" << timeoutMs << " ms)\n";

    std::vector<int> openPorts;
    for (int port = startPort; port <= endPort; ++port) {
        bool isOpen = scanPort(host, port, timeoutMs);
        if (isOpen) {
            openPorts.push_back(port);
            std::cout << "Port " << port << " ouvert\n";
        }
    }

    if (openPorts.empty()) {
        std::cout << "Aucun port ouvert détecté dans la plage.\n";
    } else {
        std::cout << "\nPorts ouverts détectés : ";
        for (size_t i = 0; i < openPorts.size(); ++i) {
            std::cout << openPorts[i];
            if (i + 1 < openPorts.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
