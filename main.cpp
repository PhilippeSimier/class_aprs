/* 
 * File:   main.cpp
 * Author: philippe SIMIER (F4JRE)
 *
 * Created on 15 septembre 2025, 11:29
 */

#include <cstdlib>
#include "AprsClient.h"
#include "Position.h"

using namespace std;

int main(int argc, char** argv) {

    try {

        AprsClient aprs;

        // Connection au serveur aprs-is
        aprs.connectToServer("euro.aprs2.net", 14580);

        // Authentification le callsign F4JRE-3 s’authentifie avec le filtre b/F4JRE-5
        aprs.authenticate("F4JRE-3", "b/F4JRE-5");

        // Création d'une position pour apparaitre sur la carte
        Position pos(48.01013, 0.20614, 85, 'I', '&', "C++ Client");  // latitude, longitude altitude symbole commentaire
        
        // Affichage du locator sur la console
        cout << "locator : " << pos.getLocator(6) << endl;
        
        // Envoi d'un beacon sur aprs-is
        aprs.sendPosition(pos);
        
        // lecture de 5 réponses
        for (int i = 0; i < 5; i++) {
            std::string resp = aprs.receiveLine();
            if (!resp.empty()) {
                std::cout << "Reçu : " << resp;
            }
            sleep(1);
        }
        
        // déconnection du serveur
        aprs.disconnect();

    } catch (const std::runtime_error& e) {
        std::cerr << "Erreur APRS : " << e.what() << std::endl;
    }


    return 0;
}

