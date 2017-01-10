#include <iostream>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <csignal>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <sys/select.h>

using namespace std;

class reqPacket
{
	private:
				uint16_t ssNum;
    			uint16_t urlLen;
    			string urlAddress;
    			string *ssChain;

    public:
			    void setHop(uint16_t value)
				{
					ssNum = value;
				}

				void setURLLength(uint16_t value)
				{
					urlLen = value;
				}

			    void setURL(char* url){
			        string temp(url);
					urlLen = temp.length();
					urlAddress = url;
			    };

			    uint16_t getHop() {

			        return ssNum;
			    };

			    uint16_t getURLLength(){
			        return urlLen;
			    };

			    string getURL(){

			        return urlAddress;
			    };

			    string& getSSAddress(int index);

				void setSSAddress(string ssAdd, int index);

			    stringstream& parseReqPacket(stringstream& in);

			    stringstream& formReqPacket(stringstream& out);

			    void ssAllocate();

			    string getNextHopIP();

			    uint16_t getNextHopPort();

};

stringstream& operator<<(stringstream& out, reqPacket &obj);

stringstream& operator>>(stringstream& in, reqPacket &obj);

ostream& operator<<(ostream& a_os, reqPacket& obj);
