#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <stdexcept>

#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string CYAN    = "\033[36m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string MAGENTA = "\033[35m";
    const std::string RED     = "\033[31m";
    const std::string GRAY    = "\033[90m";
}

enum class TLSState {
    IDLE,
    CLIENT_HELLO_SENT,
    SERVER_HELLO_SENT,
    CERTIFICATE_SENT,
    CLIENT_KEY_EXCHANGED,
    ESTABLISHED
};

std::string stateToString(TLSState state) {
    switch (state) {
        case TLSState::IDLE:                 return "IDLE";
        case TLSState::CLIENT_HELLO_SENT:    return "CLIENT_HELLO_SENT";
        case TLSState::SERVER_HELLO_SENT:    return "SERVER_HELLO_SENT";
        case TLSState::CERTIFICATE_SENT:     return "CERTIFICATE_SENT";
        case TLSState::CLIENT_KEY_EXCHANGED: return "CLIENT_KEY_EXCHANGED";
        case TLSState::ESTABLISHED:          return "ESTABLISHED";
    }
    return "UNKNOWN";
}

void pause(int ms = 700) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void printState(const std::string& who, TLSState state) {
    std::string color = (who == "Client") ? Color::CYAN : Color::MAGENTA;
    std::cout << Color::GRAY << "  [" << who << " state] "
              << Color::RESET << color << Color::BOLD
              << stateToString(state) << Color::RESET << "\n";
}

void printMessage(const std::string& from, const std::string& to,
                  const std::string& message, const std::string& color) {
    std::cout << "\n  " << color << Color::BOLD << from
              << " ──► " << to << Color::RESET
              << "  " << color << message << Color::RESET << "\n";
}

class TLSClient {
public:
    TLSState state = TLSState::IDLE;

    std::string sendClientHello() {
        if (state != TLSState::IDLE)
            throw std::logic_error("Client : état incorrect pour envoyer ClientHello");

        state = TLSState::CLIENT_HELLO_SENT;
        std::cout << Color::YELLOW << "\n  ── Étape 1 : ClientHello envoyé ──\n" << Color::RESET;
        printMessage("Client", "Serveur", "ClientHello (cipher suites, random)", Color::YELLOW);
        printState("Client", state);
        return "ClientHello";
    }

    std::string sendClientKeyExchange() {
        if (state != TLSState::CERTIFICATE_SENT)
            throw std::logic_error("Client : état incorrect pour envoyer ClientKeyExchange");

        state = TLSState::CLIENT_KEY_EXCHANGED;
        std::cout << Color::GREEN << "\n  ── Étape 3 : ClientKeyExchange envoyé ──\n" << Color::RESET;
        printMessage("Client", "Serveur", "ClientKeyExchange (pre-master secret)", Color::GREEN);
        printState("Client", state);
        return "ClientKeyExchange";
    }
};

class TLSServer {
public:
    TLSState state = TLSState::IDLE;

    std::string receiveClientHello(const std::string& message) {
        if (state != TLSState::IDLE)
            throw std::logic_error("Serveur : état incorrect pour recevoir ClientHello");
        if (message != "ClientHello")
            throw std::logic_error("Serveur : message attendu = ClientHello");

        state = TLSState::SERVER_HELLO_SENT;
        std::cout << Color::MAGENTA << "\n  ── Étape 2 : ServerHello + Certificate envoyés ──\n" << Color::RESET;
        printMessage("Serveur", "Client", "ServerHello (choix de chiffre, random)", Color::MAGENTA);
        printMessage("Serveur", "Client", "Certificate (certificat du serveur)", Color::MAGENTA);
        printState("Serveur", state);
        return "ServerHello";
    }

    void receiveClientKeyExchange(const std::string& message) {
        if (state != TLSState::SERVER_HELLO_SENT)
            throw std::logic_error("Serveur : état incorrect pour recevoir ClientKeyExchange");
        if (message != "ClientKeyExchange")
            throw std::logic_error("Serveur : message attendu = ClientKeyExchange");

        state = TLSState::ESTABLISHED;
        std::cout << Color::GREEN << "\n  ── Étape 4 : ChangeCipherSpec + Finished envoyés ──\n" << Color::RESET;
        printMessage("Serveur", "Client", "ChangeCipherSpec", Color::GREEN);
        printMessage("Serveur", "Client", "Finished", Color::GREEN);
        printState("Serveur", state);
    }
};

#ifdef _WIN32
void initConsole() {
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
#ifdef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}
#else
void initConsole() {}
#endif

int main() {
    initConsole();
    std::cout << Color::BOLD
              << "\n╔══════════════════════════════════════════╗\n"
              << "║   Simulation TLS Handshake simplifiée    ║\n"
              << "╚══════════════════════════════════════════╝\n"
              << Color::RESET;

    TLSClient client;
    TLSServer server;

    printState("Client", client.state);
    printState("Serveur", server.state);

    pause();
    std::string clientHello = client.sendClientHello();

    pause();
    std::string serverHello = server.receiveClientHello(clientHello);

    pause();
    client.state = TLSState::CERTIFICATE_SENT; // simplification : le client a reçu le certificat
    std::string clientKeyExchange = client.sendClientKeyExchange();

    pause();
    server.receiveClientKeyExchange(clientKeyExchange);

    pause();
    std::cout << "\n" << Color::GREEN << Color::BOLD
              << "  ✔ TLS handshake simulé avec succès !\n"
              << Color::RESET;
    printState("Client", client.state);
    printState("Serveur", server.state);
    std::cout << "\n";

    return 0;
}
