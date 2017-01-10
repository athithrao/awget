#include "ReqPacket.h"

using namespace std;

string& reqPacket::getSSAddress(int index)
{
    return ssChain[index];
}

void reqPacket::setSSAddress(string ssAdd, int index)
{
    ssChain[index] = ssAdd;
}

void reqPacket::ssAllocate()
{
    ssChain = new string[ssNum];
}

stringstream& reqPacket::parseReqPacket(stringstream& in){

  uint16_t i = 0;
  char delimiter;
  string temp;
  in >> i;
  ssNum = ntohs(i);
  in >> delimiter;
  in >> i;
  urlLen = ntohs(i);
  in >> delimiter;
  getline(in,urlAddress,'|');
  this->ssAllocate();
  for(i = 0; i < ssNum; i++)
  {
     getline(in,ssChain[i],'/');
  }
  
  return in;
}

/************************************************************
REQUEST PACKET GENERATION USES THE FOLLOWING SEQUENCE

SS Hop count pending(uint16_t)|URL Address Length (uint16_t)|URL Address (string)|129.82.45.59 20000/129.82.47.209 25000/...(string)|

'/' is used as a delimter to differentiate between IP Address Port combination. 

' ' is used as a delimiter in between IP Address and the Port No.

************************************************************/

stringstream& reqPacket::formReqPacket(stringstream& out){

    uint8_t i = 0;
    out << htons(ssNum-1) << '|'<< htons(urlLen) << '|' << urlAddress << '|';
    for(i = 1; i < ssNum; i++)
    {
        out << ssChain[i] << '/';
    }
    return out;

}

stringstream& operator<<(stringstream& out, reqPacket &obj) 
{
    obj.formReqPacket(out); return out;
}
stringstream& operator>>(stringstream& in, reqPacket &obj)
{
    obj.parseReqPacket(in); return in;
}

string reqPacket::getNextHopIP(){
    return (ssChain[0].substr(0,(ssChain[0].find(" "))));
}

uint16_t reqPacket::getNextHopPort(){
    string temp;
    temp = ssChain[0].substr((ssChain[0].find(" ")));
    return atoi(temp.c_str());
}

ostream &operator<<(ostream& a_os, reqPacket& obj){

    int i = 0;
    string chain;
    a_os<<"Request: "<< obj.getURL()<<endl;
    a_os<<"Chainlist is "<< endl;
    for(i=0; i < obj.getHop();i++){
        a_os<<obj.getSSAddress(i)<<endl;
    }
    a_os<<"Next SS is "<<obj.getSSAddress(0)<<endl;
    return a_os;
}