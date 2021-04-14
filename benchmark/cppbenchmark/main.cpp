
#include <string>
#include <cstdlib>
#include <thread>
#include <csignal>
#include "testclient.h"
#include "helpers.h"


using namespace std;

TestClient* testClient;

void signalHandler( int signum )
    {
    cout << "Interrupt signal (" << signum << ") received.\n";
    
    testClient->printStatistics();
    
    delete testClient;
    exit(0);
    }

int main(int argc, char **argv)
    {
    //std::cout.precision(2);
    cout << "Started at " << Helpers::now() << endl;
    //rtc::InitLogger(LogLevel::Verbose);
    string clientId = Helpers::randomString(8);
    int packetSize = 800;
	bool isSender = false;
	if (argc >= 2)
        {
        packetSize = atoi(argv[1]);
        if (string(argv[2]) == "true")
			isSender = true;
		}
        
    string wsUrl = "ws://localhost:8080/";
    if (std::getenv("WS_URL"))
        {
        wsUrl = std::getenv("WS_URL");
        cout << "env variable got";
        }
    else
        cout << "env variable not got";
    
        
    testClient = new TestClient();
    testClient->start(clientId, wsUrl, packetSize, isSender);
    
    signal(SIGINT, signalHandler);
    
    cout << "entering to sleeping loop in main"<<endl;
    while (true)
        {
        std::this_thread::sleep_for (std::chrono::milliseconds(1000));
        cout << ".";
        }
    }


