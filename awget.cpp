#include "awget.h"
#include <regex>

// random generator function:
int myrandom (int i) { return std::rand()%i;}

size_t sizeofstringstream (stringstream &data)
{
  data.seekp(0, ios::end);          
  stringstream::pos_type offset = data.tellp();
  return (size_t) offset;
}

bool awget::readSSList(char* fileName)
{
  if(fileName == NULL)
    fileName = strdup("chaingang.txt");

  ifstream myfile(fileName, ios::in);

  string line,temp;
  vector<int> ssNum;
  vector<int>::iterator it;
  uint8_t lineCounter = 0;
  srand(time(NULL));
  
  if (myfile.is_open())
    {
      getline (myfile,line);  
      packet.setHop(atoi(line.c_str()));

      for (int i=0; i<(packet.getHop()); i++) ssNum.push_back(i);

      random_shuffle ( ssNum.begin(), ssNum.end(), myrandom);

      it = ssNum.begin();

      packet.ssAllocate();

      for (int i=0; i<(packet.getHop()); i++) 
      {
        myfile.seekg (2, myfile.beg);
        lineCounter = 1;
        while ( getline (myfile,line) )
        { 
          if(lineCounter == (*it + 1))
          { 
              packet.setSSAddress(line,i);
              break;
          }
          lineCounter++;
        }
        it++;
      }
      myfile.close();
    }
  else 
  {
    cout << "Unable to open file. Quitting Program.."; 
    return false;
  }
  return true;
}

bool awget::awgetrun(){

  stringstream data;
  char buffer[MAX_PACKET_SIZE];
  int clientSocket; 
  struct sockaddr_in  clientSocketAddress;
  file = new FileManager();
  string ip = packet.getNextHopIP();

  int status;

  data << packet;

  clientSocket = socket(AF_INET, SOCK_STREAM,0);

  if(clientSocket < 0)
  {
    cout<<"Error: Socket Establishment. Quitting Program.. \n";
    return -1;
  }
    
  clientSocketAddress.sin_family = AF_INET;
  inet_pton(AF_INET,ip.c_str(),&clientSocketAddress.sin_addr.s_addr); 
  clientSocketAddress.sin_port = htons(packet.getNextHopPort());
  memset(clientSocketAddress.sin_zero,0,sizeof(clientSocketAddress.sin_zero));
    
  status = connect(clientSocket,(struct sockaddr*)&clientSocketAddress,sizeof(clientSocketAddress));
           
  if(status < 0)
  {
    cout<<"Error Connecting. Quitting Program..\n";
    close(clientSocket);
    return -1;
  }

  send(clientSocket,data.str().c_str(),sizeofstringstream(data),0);

  // Switched this section to a do .. while loop.
  // I don't get to use do while loops nearly enough,
  // and since we need a first packet before we can determine the
  // loop condition, a do .. while loop is a perfect candidate
  int packetsReceived = 0;
  do{
    recv(clientSocket,buffer,sizeof(buffer),MSG_WAITALL);
    file->acceptPacket(buffer);
    packetsReceived++;
  }while(packetsReceived < file->numPackets);

  string filename;
  findOutputName(filename);
  
  file->download(filename); // TODO: change default download name
  cout<<"Received File "<<filename<<endl;
  cout<<"Quitting...\n";
  close(clientSocket);
  
  return false;
}

/**
 * If a url consists of a filename then returns that, otherwise return the default of index.html
 * @param filename
 */
void awget::findOutputName(string& filename){
    string url = packet.getURL();
    filename = "index.html";
    size_t loc;

    loc = url.find_last_of('/');

    if(loc > 8 && url.substr(loc+1, url.length()) != ""){
        filename = url.substr(loc+1,url.length());
    }


}

void awget::printPacket(){
  cout << packet;
}


int main(int argc, char *argv[]){

    awget obj;


    string url;

    // url found
    bool urlflag = false;
    bool chainflag = false;

    if(argc > 1) {
        for (int i = 1; i < argc; i++) {

            string command = argv[i];
            if (command == "-h") {
                cout << "-----Help Information-----\n";
                cout << "To run the anonymous wget, please type ./awget [URL] {-c chainfile}\n";
                cout << "The URL is a mandatory arguement and the chainfile is optional. \n";
                exit(0);
            } else if (regex_match(command,
                                   regex("(http|https)://([^/ :]+):?([^/ ]*)(/?[^ #?]*)\\x3f?([^ #]*)#?([^ ]*)"))) {
                obj.packet.setURL(argv[i]);
                urlflag = true;
            } else if (command == "-c") {

                // custom chainlist found
                if (i + 1 < argc) {
                    if(!obj.readSSList(argv[i + 1])){
                        cerr<<"Error: Invalid chain filename"<<endl;
                        exit(-1);
                    }

                    chainflag = true;
                } else {
                    cout << "Error: To run the anonymous wget, please type ./awget [URL] {-c chainfile} \n";
                    exit(-1);
                }

            }


        }
        // no arguments found
    }else{
        cerr<<"Error: To run the anonymous wget, please type ./awget [URL] {-c chainfile} \n";
        exit(-1);
    }


    if(urlflag) {
        if(!chainflag)
            //default Chain file name
            obj.readSSList(NULL);
        cout << "awget" << endl;
        obj.printPacket();
        cout << "Waiting for the file..." << endl;
        obj.awgetrun();
    }else{
        cout<<"Error: To run the anonymous wget, please type ./awget [URL] {-c chainfile} \n";
        exit(0);
    }

  return 0;
}
