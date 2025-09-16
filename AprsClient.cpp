/* 
 * File:   AprsClient.cpp
 * Author: philippe SIMIER (F4JRE)
 * 
 * Created on 15 septembre 2025, 11:30
 */

#include "AprsClient.h"

AprsClient::AprsClient() :
sockfd(-1),
connected(false) {
}

AprsClient::AprsClient(const AprsClient& orig) {
}

AprsClient::~AprsClient() {

    try {
        disconnect();
    } catch (...) {
        // On ignore les exceptions dans le destructeur
    }
}

bool AprsClient::connectToServer(const std::string& hostname, int port) {

    struct hostent* server = gethostbyname(hostname.c_str());

    if (server == nullptr) {
        throw std::runtime_error("Impossible de résoudre le serveur : " + hostname);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("Erreur de création du socket");
    }

    struct sockaddr_in serv_addr;

    std::memset(&serv_addr, 0, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (::connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0) {

        close(sockfd);
        sockfd = -1;
        throw std::runtime_error("Erreur de connexion au serveur : " + hostname);
    }

    connected = true;
    return true;
}

bool AprsClient::authenticate(const std::string& callsign, const std::string& filter) {

    if (!connected) {
        throw std::runtime_error("Non connecté au serveur");
    }

    int pass = computePasscode(callsign);
    myCallsign = callsign; // on mémorise le callsign

    std::string line = "user " + callsign + " pass " + std::to_string(pass) + " vers C++ client";
    if (!filter.empty()) {
        line += " filter " + filter;
    }

    return sendLine(line);
}

bool AprsClient::sendLine(const std::string& line) {

    if (!connected) {
        throw std::runtime_error("Non connecté au serveur");
    }
    std::string msg = line + "\n";
    if (send(sockfd, msg.c_str(), msg.size(), 0) < 0) {
        throw std::runtime_error("Erreur lors de l'envoi de données");
    }
    return true;
}

std::string AprsClient::receiveLine() {

    if (!connected) {
        throw std::runtime_error("Non connecté au serveur");
    }

    char buffer[1024];
    std::memset(buffer, 0, sizeof (buffer));

    ssize_t n = recv(sockfd, buffer, sizeof (buffer) - 1, 0);
    if (n < 0) {
        throw std::runtime_error("Erreur lors de la réception de données");
    } else if (n == 0) {
        throw std::runtime_error("Connexion fermée par le serveur");
    }

    return std::string(buffer);

}

void AprsClient::disconnect() {

    if (connected) {
        close(sockfd);
        connected = false;

    }
}

int AprsClient::computePasscode(const std::string& callsign) {

    std::string call;

    // Ne garder que la partie avant le "-"
    size_t pos = callsign.find('-');
    if (pos != std::string::npos) {
        call = callsign.substr(0, pos);
    } else {
        call = callsign;
    }

    // En majuscules
    for (auto& c : call) {
        c = std::toupper(static_cast<unsigned char> (c));
    }

    int hash = 0x73E2;
    for (size_t i = 0; i < call.size(); i++) {
        hash ^= (call[i] << ((i & 1) ? 0 : 8));
    }

    return hash & 0x7FFF; // borne sur 15 bits 
}

/**
 * @brief envoie une balise
 * @param une référence à un objet de la classe Position
 * @return true si OK
 */
bool AprsClient::sendPosition(Position& pos) {
    
    if (myCallsign.empty()) {
        throw std::runtime_error("Aucun callsign authentifié !");
    }
    std::string headAprs = myCallsign + ">APRS:";
    return sendLine(headAprs + pos.getPduAprs(true));
}

