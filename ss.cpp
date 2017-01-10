#include "ss.h"
#include <errno.h>
#include <stdlib.h>

volatile sig_atomic_t stop;

int masterSocket = -1;
int clientSocket[MAXSSCLIENTS];
int toSSHostSocket[MAXSSCLIENTS];
int newSocket = -1;

void sig_handler(int);

void ss::printPacket(){
  	cout << packet;
}

size_t sizeofstringstream (stringstream &data)
{
  data.seekp(0, ios::end);          
  stringstream::pos_type offset = data.tellp();
  return (size_t) offset;
}

 void ss::sswget(int socket)
 {
    char buffer[MAX_PACKET_SIZE];

    file->fetch(packet.getURL());
    
    cout<<"File Received..\n";

    file->createPackets();

    cout<<"Relaying File...\n";

    memset(buffer,0,sizeof(buffer));

    for(int i =0; i< file->numPackets;i++) {
        //cout<<sizeof(&(file->packets[i]))<<endl;
        memcpy(buffer, file->packets[i], sizeof(buffer));
        send(socket, buffer, sizeof(buffer), 0);
        memset(buffer,0,sizeof(buffer));
    }
    cout<<"File Packets transmitted..\n";
    cout<<"Press ctrl+C to Quit or wait for the next awget request...\n";
 }

 int ss::toSSConnect()
 {
    struct sockaddr_in address;
    int newSocket = 0;
    int status = 0;
    string ip = packet.getNextHopIP();
    stringstream data;

    address.sin_family = AF_INET;
    inet_pton(AF_INET,ip.c_str(),&address.sin_addr.s_addr); 
    address.sin_port = htons(packet.getNextHopPort());
    memset(address.sin_zero,0,sizeof(address.sin_zero));

    data << packet;

    newSocket = socket(AF_INET, SOCK_STREAM,0);

    if(newSocket < 0)
    {
        cout<<"Error: Socket Establishment. Quitting Program.. \n";
        return -1;
    }
    
    status = connect(newSocket,(struct sockaddr*)&address,sizeof(address));
           
    if(status < 0)
    {
        cout<<"Error Connecting. Quitting Program.. \n";
        close(newSocket);
        return -1;
    }

    cout<<"Connection Established.\n";

    send(newSocket,data.str().c_str(),sizeofstringstream(data),0);

    cout<<"Request Packet Sent."<<endl;

    return newSocket;
 }

bool ss::ssrun(int portNum)
{
    int option = 1;
	int status;
    int max_sd,sd;
    int i = 0;
    fd_set readfds;

    file = new FileManager();
    struct sockaddr_in sin,clientSocketAddress;
    int defaultPort = 15000;
    char buffer[MAX_PACKET_SIZE];
    socklen_t socketAddLen;

    stringstream reqData;

    for (i = 0; i < MAXSSCLIENTS; i++) 
    {
        clientSocket[i] = 0;
        toSSHostSocket[i] = 0;
    }
    
    masterSocket = socket(AF_INET, SOCK_STREAM,0);
    
    if(masterSocket < 0)
    {
        cout<<"Error: Socket Establishment. Quitting Program.. \n";
        exit(1);
    }

    if( setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option)) < 0 )
    {
        cout<<"Error: Socket to alloe multiple connections. Quitting Program.. \n";
        exit(1);
    }
    
    bzero((char *)&sin,sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    if(portNum == 0)
        sin.sin_port = htons(defaultPort);
    else
        sin.sin_port = htons(portNum);

    socklen_t size = sizeof(sin);
    
    status = bind(masterSocket,(struct sockaddr*)&sin,size);
    
    if(masterSocket < 0)
    {
        cout<<"Error Binding. Quitting Program.. \n";
        close(masterSocket);
        exit(1);
    }
    
    cout<<"SS <"<< getAddress(std::to_string(portNum))<<","<<ntohs(sin.sin_port)<<">\n";
    
    cout<<"Waiting for Connections "<<endl;
    
    listen(masterSocket,MAXSSCLIENTS);
    
    socketAddLen = sizeof(clientSocketAddress);

    while(!stop)
    {
        signal(SIGINT,sig_handler);
        
        FD_ZERO(&readfds);
  
        FD_SET(masterSocket, &readfds);
        max_sd = masterSocket;

        for (i = 0; i < MAXSSCLIENTS; i++) 
        {
            sd = toSSHostSocket[i];

            if(sd > 0)
                FD_SET(sd, &readfds);

            if(sd > max_sd)
                max_sd = sd;
        }

        status = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((status < 0) && (errno!=EINTR)) 
        {
            cout<<"Select function error. Quitting Program..\n";
            exit(1);
        }

        if (FD_ISSET(masterSocket, &readfds)) 
        {
            
            if ((newSocket = accept(masterSocket,(struct sockaddr*)&clientSocketAddress,&socketAddLen)) < 0)
            {
                cout<<"New Socket acceptance error. Quitting Program..\n";
                exit(1);
            }
          
            memset(buffer,0,sizeof(buffer));

            //send new connection greeting message
            recv(newSocket,buffer,sizeof(buffer),0);
                            
            reqData << buffer;

            packet.parseReqPacket(reqData);

            if(packet.getHop() == 0)
            {   
                string filename;
                cout<<"Packet URL"<<packet.getURL()<<endl;
                cout<<"Chainlist is empty...\n";
                findOutputName(filename);
                cout<<"issuing wget for file "<<filename<<".\n";
                sswget(newSocket);
                close(newSocket);
            }
            else
            {
                for (i = 0; i < MAXSSCLIENTS; i++) 
                {
                    //if position is empty
                    if( clientSocket[i] == 0 )
                    {
                        clientSocket[i] = newSocket;
                        break;
                    }
                }
                printPacket();
                toSSHostSocket[i] = toSSConnect();
            }
        }
        for (i = 0; i < MAXSSCLIENTS; i++) 
        {
            sd = toSSHostSocket[i];
              
            if (FD_ISSET( sd , &readfds)) 
            {
                //Check if it was for closing , and also read the incoming message
                int seqNum, numPackets;

                memset(buffer,0,sizeof(buffer));

                recv(sd,buffer,sizeof(buffer),MSG_WAITALL);

                memcpy(&seqNum, buffer, sizeof(seqNum));
                memcpy(&numPackets, buffer + sizeof(seqNum), sizeof(numPackets));

                seqNum = ntohl(seqNum);
                numPackets = ntohl(numPackets);

                if(seqNum < numPackets)
                {
                    send(clientSocket[i],buffer,sizeof(buffer),0);

                    if ((numPackets-seqNum) == 1)
                    {
                        close(clientSocket[i]);
                        close(sd);
                        toSSHostSocket[i] = 0;
                        clientSocket[i] = 0;
                    }

                }
            }
        }
    }
    
    close(masterSocket);

    return true;
}

void sig_handler(int signal)
{
    //close all open sockets and any allocated memory.
    stop = 1;
    close(masterSocket);
    close(newSocket);
    for(int i = 0; i< MAXSSCLIENTS;i++)
    {
        close(clientSocket[i]);
        close(toSSHostSocket[i]);
    }
    cout<<"Quiting...\n";
    exit(0);
}

void ss::findOutputName(string& filename){
    string url = packet.getURL();
    filename = "index.html";
    size_t loc;

    loc = url.find_last_of('/');

    if(loc != url.size()){
        filename = url.substr(loc,url.length());
    }


}

std::string ss::getAddress(std::string port) {

    char hostname[128];


    gethostname(hostname, sizeof hostname);
    printf("My hostname: %s\n", hostname);


    struct addrinfo hints, *servinfo, *p,*res;
    int status;

    char ipstr[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(hostname,"50685",&hints,&servinfo);


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));

    }



    for(p = res;p != NULL; p = p->ai_next) {
        void *addr = NULL;


        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        //printf("  %s: %s\n", ipver, ipstr);
    }

    freeaddrinfo(res); // free the linked list


    return ipstr;
}


int main(int argc, char *argv[]){

	ss obj;

    bool portFlag = false;
    int portDig = 15000;
    //use default port
    if(argc>3){

        cerr<<"Invalid number of arguments"<<endl;
        cout<<"Usage: $ss [-p port]"<<endl;
        return -1;

    }else if(argc == 3){

        //parse argument's for port

        for(int i = 1;i<argc; i++) {

            string value = argv[i];
            if (i + 1 < argc) {
                if (argc < i + 1) {
                    cerr << "Invalid value for the port" << endl;
                    cout<<"Usage: $ss [-p port]"<<endl;
                    return -1;
                }

                string port = argv[i + 1];
                if (value == "-p") {
                    portFlag = true;

                    //check if port is a valid digit
                    // if return is -1 then the digit contains more than just numbers
                    if (port.find_first_not_of("01234567890") != std::string::npos) {
                        cerr << "Invalid value for the port" << endl;
                        cout<<"Usage: $ss [-p port]"<<endl;
                        return -1;
                    }

                    portDig = atoi(port.c_str());


                    if (portDig < 1024 || portDig > 65535) {
                        cerr << "Invalid port number: " << port << endl;
                        cout<<"Usage: $ss [-p port]"<<endl;
                        return -1;
                    }

                }
        }

        }


    }


    if(!portFlag && argc >1){
        cout<<"Usage: $ss [-p port]"<<endl;
        return -1;
    }
    obj.ssrun(portDig);

}
