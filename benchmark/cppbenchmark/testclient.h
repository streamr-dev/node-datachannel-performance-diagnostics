//
//  testclient.hpp
//  datachannel
//
//  Created by Savolainen, Petri T E on 02/03/2021.
//

#ifndef testclient_hpp
#define testclient_hpp

#include <stdio.h>
#include <string>
#include <map>

#include "rtc/rtc.hpp"
#include "peer.h"

#define PACKET_SIZE 800
#define SENDING_INTERVAL 2
#define STATS_INTERVAL 1000

using namespace std;

class TestClient
{
private:

map<string, shared_ptr<Peer> > connections;
shared_ptr<WebSocket> ws;

public:

TestClient();
void start(const string& id, const string& wsUrl, int packetSize, bool isSender);
void doSending();
void printStatistics();
    
};

#endif /* testclient_hpp */
