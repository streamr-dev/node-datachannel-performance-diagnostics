//
//  peer.h
//  libdatachannel
//
//  Created by Savolainen, Petri T E on 01/03/2021.
//

#ifndef peer_h
#define peer_h

#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include "rtc/rtc.hpp"

/*
#define BUFFER_LOW 1<<20
#define BUFFER_HIGH 1<<24
*/

#define BUFFER_LOW 262144
#define BUFFER_HIGH 1048576


#define INTERVAL 2

using namespace std;
using namespace rtc;
using json = nlohmann::json;

struct Statistic {std::uint64_t fistPacketAt; std::uint64_t lastPacketAt; std::uint64_t bytesReceived; };
class Peer
{
private:
    string selfId;
    string peerId;
    string logId;
    string message;
    
    // the key is the packet size
    
    map<int, Statistic> stats;
    
    int packetSize;
    bool isSender;

    shared_ptr<PeerConnection> connection;
    shared_ptr<DataChannel> dc;
    shared_ptr<WebSocket> signalingServerWs;
    bool paused = true;
    
    PeerConnection::State lastState;
    int bytesIn = 0;
    int bytesOut = 0;
    int bytesFailed = 0;
    
    shared_ptr<PeerConnection> createPeerConnection(const Configuration &config, weak_ptr<WebSocket> wws, string id);
    void setUpDataChannel(shared_ptr<DataChannel> dc);
    void sendRelay(json& data);
    
public:
    Peer(string selfId, string peerId, shared_ptr<WebSocket> signalingServerWs, int _packetSize, bool _isSender);

    void startAsActive();
    void startAsPassive();
    
    void handleRemoteData(json data);
    bool publish(const string& message);
    
    string getLogId();
    int getBufferedAmount();
    PeerConnection::State getState();
    bool isOpen();
    int getBytesIn();
    int getBytesOut();
    int getBytesFailed();
    void resetCounters();
    string getStatsAsString();
};


#endif /* peer_h */
