//
//  testclient.cpp
//  datachannel
//
//  Created by Savolainen, Petri T E on 02/03/2021.
//

#include "testclient.h"
#include <nlohmann/json.hpp>
#include <future>

#include "peer.h"
#include "helpers.h"

using namespace Helpers;

using json = nlohmann::json;

TestClient::TestClient() {}

void TestClient::start(const string& id, const string& wsUrl, int packetSize, bool isSender)
    {
    this->ws = make_shared<WebSocket>();

    std::promise<void> wsPromise;
    auto wsFuture = wsPromise.get_future();

    ws->onOpen([&wsPromise]()
        {
        cout << now() << "Connected to signaling server; waiting for further messages from signaling server..." << endl;
        wsPromise.set_value();
        });

    ws->onMessage([connections = &this->connections, id, ws=&this->ws, packetSize, isSender] (variant<binary, string> data)
        {
        if (!holds_alternative<string>(data))
            {
            cout << now() << "Error: received websocket message that is not a string" << endl;
            return;
            }
               
        json message = json::parse(get<string>(data));

        auto it = message.find("type");
        if (it == message.end())
            {
            cout << now() << "Error: received websocket message has no type field" << endl;
            return;
            }
        
        string type = it->get<string>();
        
        if (type =="connect")
            {
            auto it2 = message.find("target");
            if (it2 == message.end())
                {
                cout << now() << "Error: received websocket connect message has no target field" << endl;
                return;
                }
            string target = it2->get<string>();
            
            shared_ptr<Peer> peer = make_shared<Peer>(id, target, *ws, packetSize, isSender);
            
            connections->emplace(target, peer);
            connections->find(target)->second->startAsActive();
            }
        
        else if (type == "relay")
            {
            auto it3 = message.find("from");
            if (it3 == message.end())
                {
                cout << now() << "Error: received websocket relay message has no from field" << endl;
                return;
                }
                
            string from = it3->get<string>();
            
            cout << now() << "Relay message received signaling server, from=" << from << endl;
            
            if (connections->find(from) == connections->end())
                {
                cout << now() << "Creating passive connection for " <<from <<endl;
                connections->emplace(from,  make_shared<Peer>(id, from, *ws, packetSize, isSender));
                connections->find(from)->second->startAsPassive();
                }
                
            auto it4 = message.find("data");
            if (it4 == message.end())
                {
                cout << now() << "Error: received websocket relay message has no data field" << endl;
                return;
                }
                
            json data = it4->get<json>();
            connections->find(from)->second->handleRemoteData(data);
            }
        else
            {
            cout << now() << "Error: received websocket message had an unknown type: " << type << endl;
            return;
            }
       });

        
    ws->onError([&wsPromise](string s)
        {
        cout << now() << "WebSocket error" << endl;
        wsPromise.set_exception(std::make_exception_ptr(std::runtime_error(s)));
        });

    ws->onClosed([]()
        {
        //cout << now() << "Connection lost to signaling server" << endl;
        exit(0);
        });
        
    ws->open(wsUrl+"?id="+id);

    cout << now() << "Waiting for signaling to be connected..." << endl;
    wsFuture.get();

    cout << now() << "Connected to the signalling server" << endl;
        
        
    /*
    long counter = 0;
    while (true)
        {
        std::this_thread::sleep_for (std::chrono::milliseconds(1));
        counter++;
        
        if (counter % 2 == 0)
            {
            this->doSending();
            }
            
        if (counter % 1000 == 0)
            {
            this->printStatistics();
            }
        }
    */
    }

/*
void TestClient::doSending()
    {
    string packet = Helpers::randomString(PACKET_SIZE);
    
    for (auto const& [key, val] : this->connections)
        {
        val->publish(packet);
        }
    }
*/
 
void TestClient::printStatistics()
    {
    /*
    int totalIn = 0;
    int totalOut = 0;
    int totalFailed = 0;
    int totalBufferedAmount = 0;
    */
    for (auto const& [key, conn] : this->connections)
        {
        cout << conn->getStatsAsString() << "\n";
            /*
        totalIn += conn->getBytesIn();
        totalOut += conn->getBytesOut();
        totalFailed += conn->getBytesFailed();
        totalBufferedAmount += conn->getBufferedAmount();
        
        cout << conn->getLogId() <<" rate " << Helpers::formatRate(conn->getBytesIn()) <<" / " << Helpers::formatRate(conn->getBytesOut()) <<" kb/s "
            << "("  <<Helpers::formatRate(conn->getBytesFailed()) << "," << Helpers::formatRate(conn->getBufferedAmount())<<", "<< conn->getState() << ", " << conn->isOpen() << ")"<<endl;
        
        conn->resetCounters();
            */
        }
        
        /*
        cout << now() << "Total " << Helpers::formatRate(totalIn) <<" / " << Helpers::formatRate(totalOut) << " kb/s (" << Helpers::formatRate(totalFailed) << "," << Helpers::formatRate(totalBufferedAmount) << ")" << endl;
    */
    }
    
    
