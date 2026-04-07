#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

// ─── Couleurs ANSI ───────────────────────────────────────────────────────────
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

// ─── États TCP ───────────────────────────────────────────────────────────────
enum class TCPState {
    CLOSED,
    LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED
};

std::string stateToString(TCPState state) {
    switch (state) {
        case TCPState::CLOSED:        return "CLOSED";
        case TCPState::LISTEN:        return "LISTEN";
        case TCPState::SYN_SENT:      return "SYN_SENT";
        case TCPState::SYN_RECEIVED:  return "SYN_RECEIVED";
        case TCPState::ESTABLISHED:   return "ESTABLISHED";
    }
    return "UNKNOWN";
}

// ─── Structure d'un segment TCP simplifié ────────────────────────────────────
struct TCPSegment {
    bool     SYN = false;
    bool     ACK = false;
    uint32_t seq = 0;   // numéro de séquence
    uint32_t ack = 0;   // numéro d'acquittement
};

std::string segmentToString(const TCPSegment& seg) {
    std::string flags;
    if (seg.SYN) flags += "SYN ";
    if (seg.ACK) flags += "ACK ";
    if (flags.empty()) flags = "(no flags)";
    return "[" + flags + "| seq=" + std::to_string(seg.seq)
               + " ack=" + std::to_string(seg.ack) + "]";
}

// ─── Helpers d'affichage ─────────────────────────────────────────────────────
void pause(int ms = 600) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void printState(const std::string& who, TCPState state) {
    std::string color = (who == "Client") ? Color::CYAN : Color::MAGENTA;
    std::cout << Color::GRAY << "  [" << who << " state] "
              << Color::RESET << color << Color::BOLD
              << stateToString(state) << Color::RESET << "\n";
}

void printArrow(const std::string& from, const std::string& to,
                const TCPSegment& seg, const std::string& color) {
    std::cout << "\n  " << color << Color::BOLD << from
              << " ──► " << to << Color::RESET
              << "  " << color << segmentToString(seg) << Color::RESET << "\n";
}

void printSeparator() {
    std::cout << Color::GRAY
              << "  ────────────────────────────────────────────\n"
              << Color::RESET;
}

// ─── Classe Client ────────────────────────────────────────────────────────────
class Client {
public:
    TCPState state = TCPState::CLOSED;
    uint32_t isn   = 100;   // Initial Sequence Number (simplifié)

    // Étape 1 : envoie SYN
    TCPSegment sendSYN() {
        if (state != TCPState::CLOSED)
            throw std::logic_error("Client : état incorrect pour envoyer SYN");

        TCPSegment seg;
        seg.SYN = true;
        seg.seq = isn;
        state = TCPState::SYN_SENT;

        std::cout << Color::YELLOW << "\n  ── Étape 1 : le client envoie SYN ──\n" << Color::RESET;
        printArrow("Client", "Serveur", seg, Color::YELLOW);
        printState("Client", state);
        return seg;
    }

    // Étape 3 : reçoit SYN-ACK, envoie ACK
    TCPSegment receiveSYNACK(const TCPSegment& synack) {
        if (state != TCPState::SYN_SENT)
            throw std::logic_error("Client : SYN-ACK reçu dans un mauvais état");
        if (!synack.SYN || !synack.ACK)
            throw std::logic_error("Client : segment attendu = SYN+ACK");
        if (synack.ack != isn + 1)
            throw std::logic_error("Client : numéro d'acquittement incorrect");

        TCPSegment seg;
        seg.ACK = true;
        seg.seq = isn + 1;
        seg.ack = synack.seq + 1;
        state = TCPState::ESTABLISHED;

        std::cout << Color::GREEN << "\n  ── Étape 3 : le client envoie ACK ──\n" << Color::RESET;
        printArrow("Client", "Serveur", seg, Color::GREEN);
        printState("Client", state);
        return seg;
    }
};

// ─── Classe Serveur ───────────────────────────────────────────────────────────
class Server {
public:
    TCPState state = TCPState::LISTEN;
    uint32_t isn   = 300;   // ISN côté serveur

    // Étape 2 : reçoit SYN, envoie SYN-ACK
    TCPSegment receiveSYN(const TCPSegment& syn) {
        if (state != TCPState::LISTEN)
            throw std::logic_error("Serveur : pas en écoute");
        if (!syn.SYN)
            throw std::logic_error("Serveur : segment SYN attendu");

        TCPSegment seg;
        seg.SYN = true;
        seg.ACK = true;
        seg.seq = isn;
        seg.ack = syn.seq + 1;
        state = TCPState::SYN_RECEIVED;

        std::cout << Color::MAGENTA << "\n  ── Étape 2 : le serveur envoie SYN-ACK ──\n" << Color::RESET;
        printArrow("Serveur", "Client", seg, Color::MAGENTA);
        printState("Serveur", state);
        return seg;
    }

    // Finalisation : reçoit ACK, connexion établie
    void receiveACK(const TCPSegment& ack_seg) {
        if (state != TCPState::SYN_RECEIVED)
            throw std::logic_error("Serveur : ACK reçu dans un mauvais état");
        if (!ack_seg.ACK)
            throw std::logic_error("Serveur : segment ACK attendu");
        if (ack_seg.ack != isn + 1)
            throw std::logic_error("Serveur : numéro d'acquittement incorrect");

        state = TCPState::ESTABLISHED;
        printState("Serveur", state);
    }
};

// ─── Console Windows ─────────────────────────────────────────────────────────
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

// ─── Programme principal ──────────────────────────────────────────────────────
int main() {
    initConsole();
    std::cout << Color::BOLD
              << "\n╔══════════════════════════════════════════╗\n"
              << "║   Simulation TCP Three-Way Handshake     ║\n"
              << "╚══════════════════════════════════════════╝\n"
              << Color::RESET;

    std::cout << "\n  Client ISN : 100  |  Serveur ISN : 300\n";
    printSeparator();

    try {
        Client client;
        Server server;

        // ── Affichage des états initiaux ──
        std::cout << "\n  États initiaux :\n";
        printState("Client", client.state);
        printState("Serveur", server.state);

        // ── Étape 1 : SYN ────────────────
        pause();
        TCPSegment syn = client.sendSYN();

        // ── Étape 2 : SYN-ACK ────────────
        pause();
        TCPSegment synack = server.receiveSYN(syn);

        // ── Étape 3 : ACK ─────────────────
        pause();
        TCPSegment ack = client.receiveSYNACK(synack);

        // ── Serveur enregistre la connexion ──
        server.receiveACK(ack);

        // ── Résultat final ──────────────────
        printSeparator();
        std::cout << Color::GREEN << Color::BOLD
                  << "\n  ✔ Connexion établie avec succès !\n"
                  << Color::RESET;
        std::cout << "\n  États finaux :\n";
        printState("Client", client.state);
        printState("Serveur", server.state);

        std::cout << "\n  Les deux parties peuvent maintenant\n"
                     "  échanger des données de manière fiable.\n\n";

    } catch (const std::exception& e) {
        std::cerr << Color::RED << "\n  ✘ Erreur : " << e.what()
                  << Color::RESET << "\n\n";
        std::cout << "Appuyez sur Entrée pour quitter...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return 1;
    }

    std::cout << "Appuyez sur Entrée pour quitter...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return 0;
}