/* 
 * File:   AprsClient.h
 * Author: philippe SIMIER (F4JRE)
 *
 * Created on 15 septembre 2025, 11:30
 */

#ifndef APRSCLIENT_H
#define APRSCLIENT_H

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "Position.h"

class AprsClient {

public:
    AprsClient();
    AprsClient(const AprsClient& orig);
    virtual ~AprsClient();
    
    bool connectToServer(const std::string& hostname, int port);
    bool authenticate(const std::string& callsign, const std::string& filter = "");
    bool sendPosition(Position& pos);
    bool sendLine(const std::string& line);
    std::string receiveLine();
    void disconnect();

private:
    int sockfd;
    bool connected;
    std::string myCallsign;
    int computePasscode(const std::string& callsign);
};

#endif /* APRSCLIENT_H */

