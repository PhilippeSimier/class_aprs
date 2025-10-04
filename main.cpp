/* 
 * File:   main.cpp
 * Author: philippe SIMIER (F4JRE)
 * 
 * Programme Exemple d'utilisation de la class AprsClient
 * 
 *  gestion du signal SIGINT (Ctrl+C)
 *  arrêt propre du thread d’écoute
 *  fermeture du socket APRS-IS
 *  affichage clair des événements
 * 
 *  Created on 15 septembre 2025, 11:29
 */

#include <iostream>
#include <thread>
#include <csignal>
#include "AprsClient.h"
#include "Position.h"

using namespace std;

static bool stopRequested = false;

// Gestionnaire du signal SIGINT (Ctrl+C)
void signalHandler(int signal);

// Fonction callback appelée à chaque ligne reçue du serveur APRS-IS
void onAprsMessageReceived(const std::string& message);

int main() {

    try {
        // --- 1️Création du client APRS-IS ---
        AprsClient aprs;

        // --- 2️Connexion au serveur APRS-IS ---
        aprs.connectToServer("euro.aprs2.net", 14580);

        // --- 3️ Authentification avec un filtre---
        aprs.authenticate("F4JRE-3", "r/48.01013/0.20614/100");

        // --- 4️ Création d'une position pour apparaître sur la carte APRS ---
        Position pos(48.01013, 0.20614, 85, 'I', '&', "C++ Client"); // latitude, longitude, altitude, symbole, commentaire

        // --- 5️ Affichage du locator sur la console ---
        cout << "Locator : " << pos.getLocator(6) << endl;

        // --- 6️ Envoi d'un beacon initial ---
        aprs.sendPosition(pos);
        cout << "[APRS] Beacon initial envoyé." << endl;

        // --- 7️ Démarrage du thread d’écoute APRS-IS ---
        aprs.startListening(onAprsMessageReceived);

        // --- 8️ Boucle principale ---
        while (!stopRequested) {

            this_thread::sleep_for(chrono::minutes(5));
            if (stopRequested) break;

            try {
                aprs.sendPosition(pos);
                cout << "[APRS] Beacon périodique envoyé." << endl;
            } catch (const runtime_error& e) {
                cerr << "[APRS] Erreur d’envoi : " << e.what() << endl;
            }
        }

        // --- 🔟 Arrêt propre ---
        cout << "[APRS] Arrêt du thread d’écoute et fermeture du socket..." << endl;
        aprs.stopListening();
        aprs.disconnect();
        cout << "[APRS] Déconnexion terminée. Fin du programme." << endl;
    } catch (const runtime_error& e) {
        cerr << "Erreur fatale APRS : " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

void signalHandler(int signal) {
    if (signal == SIGINT) {
        cout << "\n[APRS] Arrêt demandé (SIGINT reçu)..." << endl;
        stopRequested = true;
    }
}

void onAprsMessageReceived(const std::string& message) {
    
    if (!message.empty() && message[0] == '#') {
        return; // ignore toutes les lignes de commentaires serveur
    }

    std::cout << "[APRS RX] " << message;
}

