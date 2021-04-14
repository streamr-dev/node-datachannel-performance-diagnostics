//
//  peer.cpp
//  datachannel
//
//  Created by Savolainen, Petri T E on 01/03/2021.
//

#include <stdio.h>
#include "peer.h"
#include "helpers.h"

using namespace Helpers;

Peer::Peer(string _selfId, string _peerId, shared_ptr<WebSocket> _signalingServerWs, int _packetSize, bool _isSender):
	selfId(_selfId), peerId(_peerId), signalingServerWs(_signalingServerWs), packetSize(_packetSize), isSender(_isSender)
	{
	this->logId = this->selfId + "->" + this->peerId;
	string stunServer = "stun:stun.l.google.com:19302";
	
    this->message = Helpers::randomString(packetSize);
        
    Configuration config;
	config.iceServers.emplace_back(stunServer);
	this->connection = make_shared<PeerConnection>(config);
		
	this->connection->onStateChange([this](PeerConnection::State state)
		{
		//cout << now() << "State: " << state << endl;
		this->lastState = state;
		});
	
   
	this->connection->onLocalDescription([_signalingServerWs, _selfId, _peerId](Description description)
		{
		json data =  {{"id", _selfId}, {"type", description.typeString()}, {"description", string(description)}};
		json message =
			{
				{"type", "relay"},
				{"to", _peerId},
				{"data", data}
			};
		
		_signalingServerWs->send(message.dump());
		});
	   
	this->connection->onLocalCandidate([_signalingServerWs, _selfId, _peerId](Candidate candidate)
		{
		json data = { {"candidate", candidate}, {"mid",candidate.mid()} };
		
		json message =
				   {
					   {"type", "relay"},
					   {"to", _peerId},
					   {"data", data}
				   };
			   
		_signalingServerWs->send(message.dump());
		});
   }

void Peer::setUpDataChannel(shared_ptr<DataChannel> dc)
	{
	this->dc = dc;    
	dc->setBufferedAmountLowThreshold(BUFFER_LOW);
		
	dc->onOpen([obj = this, logId = this->logId, peerId = this->peerId]()
		{
		cout << now() << logId << " !!!!!!!!!!!!!!!!!!!!!!!!!! DataChannel " << peerId << " open, isSender: "<< obj->isSender << endl;
        if (obj->isSender)
            {
            obj->publish(obj->message);
            }
        });
		
	dc->onClosed([logId = this->logId, peerId = this->peerId]()
		{
		cout << now() << logId << " DataChannel " << peerId << " closed" << endl;
		});
		
	dc->onError([logId = this->logId, peerId = this->peerId](string err)
		{
		cout << now() << logId << " DataChannel " << peerId << " error: "<< err << endl;
		});
		
    if (this->isSender)
        {
            
        cout<<"setting onBufferedAmountLow callback"<<endl;
        dc->onBufferedAmountLow([obj = this, logId = this->logId, peerId = this->peerId, dc= this->dc] ()
            {
            //cout << now() << logId << " DataChannel " << peerId << " LOW buffer " <<  dc->bufferedAmount()<< endl;
            obj->publish(obj->message);
            });
        }
    
	dc->onMessage([obj=this] (message_variant msg)
		{
        //cout << "onMessage" << endl;
		if (holds_alternative<string>(msg))
			{
            
            uint64_t size = get<string>(msg).size();
			uint64_t now = Helpers::millisecondsNow();
           
			
            if (obj->stats.find(size) == obj->stats.end())
                {
                
                struct Statistic stat = {now, now, size};
                
                obj->stats.emplace(size, stat);
                }
            else
                {
                auto stat = obj->stats.at(size);
                stat.bytesReceived += size;
                stat.lastPacketAt = now;
                
                obj->stats[size] = stat;
                }
            obj->bytesIn += size;
            }
		});
        cout << "setUpDataChannel completed" << endl;
	}

void Peer::startAsActive()
	{
    cout << "startAsActive()" << endl;
	if (this->dc)
		{
		throw runtime_error("Already started!");
		}
	
	this->dc =  this->connection->createDataChannel("generalDataChannel");
		
	this->setUpDataChannel(this->dc);
    //if (this->isSender)
     //   {
     //   this->publish(this->message);
     //   }
	//dc->onOpen([obj=this]()
	//	{
	//	obj->paused = false;
	//	});
	}

void Peer::startAsPassive()
	{
    cout << "startAsPassive()" << endl;
	if (this->dc)
		{
		throw runtime_error("Already started!");
		}
		
	this->connection->onDataChannel([this](shared_ptr<DataChannel> dc)
		{
		cout << now() << "DataChannel received with label \"" << dc->label() << "\"" << endl;

		this->setUpDataChannel(dc);
        
        if (this->isSender)
            {
            this->publish(this->message);
            }
        
        //this->paused = false;
		});
	}

void Peer::handleRemoteData(json data)
	{
	string candidate = "";
	string mid = "";
	string description = "";
	string type = "";
		
	auto it = data.find("candidate");
	if (it != data.end())
		{
		candidate = it->get<string>();
		}
   
	auto it2 = data.find("mid");
	if (it2 != data.end())
		{
		mid = it2->get<string>();
		}
   
	auto it3 = data.find("description");
	if (it3 != data.end())
		{
		description = it3->get<string>();
		}
		   
	auto it4 = data.find("type");
	if (it4 != data.end())
		{
		type = it4->get<string>();
		}
	
	if (candidate != "" && mid !="")
		{
		this->connection->addRemoteCandidate(candidate);
		}
	
   else if (description != "" && type!= "")
		{
		Description desc(description, type);
		this->connection->setRemoteDescription(desc);
		}
   else
		{
		cout << now() << this->logId << " unrecognized RTC message: " << data << endl;
		}
	}

bool Peer::publish(const string& message)
	{
    //cout << "publish";
    /*
	if (this->paused)
		{
		this->bytesFailed += message.size();
		return false;
		}
		
	if (this->dc->bufferedAmount() >= BUFFER_HIGH)
		{
		this->paused = true;
		cout << now() << this->logId << " DataChannel HIGH buffer " << this->dc->bufferedAmount() <<endl;
		this->bytesFailed += message.size();
		return false;
		}
		
	try 
		{
		this->dc->send(message);
		} 
	catch (const std::exception& e) 
		{
		cout << now() << this->logId << " send failed: " << e.what() << endl;
		this->bytesFailed += message.size();
		}

	this->bytesOut += message.size();
	return true;
	*/
    
    while (this->dc->bufferedAmount() < BUFFER_HIGH)
        {
        try
            {
           
            auto result = this->dc->send(message);
           
            this->bytesOut += message.size();
            }
        catch (const std::exception& e)
            {
            cout << now() << this->logId << " send failed: " << e.what() << endl;
            break;
            }
        }
        
    return true;
    }

string Peer::getLogId()
	{
	return this->logId;
	}

int Peer::getBufferedAmount()
	{
	return this->dc ? this->dc->bufferedAmount() : 0;
	}

PeerConnection::State Peer::getState()
	{
	return this->lastState;
	}

bool Peer::isOpen()
	{
	return this->dc ? this->dc->isOpen() : false;
	}

int Peer::getBytesIn()
	{
	return this->bytesIn;
	}

int Peer::getBytesOut()
	{
	return this->bytesOut;
	}

int Peer::getBytesFailed()
	{
	return this->bytesFailed;
	}

void Peer::resetCounters()
	{
	this->bytesIn = 0;
	this->bytesOut = 0;
	this->bytesFailed = 0;
	}

string Peer::getStatsAsString()
    {
    cout<<"getStatsAsString()"<<endl;
    string ret = "";
        
    for (auto it = this->stats.begin(); it != this->stats.end(); it++)
        {
        //cout << (*it).first << endl;
        auto stat = (*it).second;
        //cout << stat.lastPacketAt << "-" << stat.fistPacketAt << endl;
        uint64_t timeSpent = stat.lastPacketAt - stat.fistPacketAt;
        
            
        //cout << "timeSpentti: " << to_string(timeSpent) <<endl;
        //cout << "bytesReceived: " << stat.bytesReceived <<endl;
            
        double bytesPerSecond = (static_cast<double>(stat.bytesReceived) / timeSpent) * 1000;
        double kilobytesPerSecond = (bytesPerSecond / 1024);
        double megabitsPerSecond = kilobytesPerSecond * 8 / 1024;
        //cout << kilobytesPerSecond << endl;
        
        ret += to_string((*it).first);
        ret += "    ";
        ret += to_string(kilobytesPerSecond);
        ret += "    ";
        ret += to_string(kilobytesPerSecond*8);
        ret += "    ";
        ret += to_string(megabitsPerSecond);
        ret += "\n";
        ret += to_string(megabitsPerSecond);
        }
    //cout << "ret: " << ret << endl;
        
    return ret;
    }


