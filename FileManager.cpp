#include "FileManager.h"
#include <arpa/inet.h>

FileManager::FileManager() {
	numPackets = -1;
}

void FileManager::fetch(string url) {
	filename = "tempDownloadFile";
	string command;
	command = "wget " + url + " -O " + filename;
	system(command.c_str());

	if(filename.size() == 0) {
		cerr << "Error downloading file" << endl;
	}
}

void FileManager::download(string path) {
	int maxDataChunkSize = MAX_PACKET_SIZE - sizeof(int) - sizeof(numPackets);
	ofstream fileStream;
	fileStream.open(path);
	for(int i = 0; i < numPackets; i++) {
		if(i == numPackets - 1) {
			int j = MAX_PACKET_SIZE - 1;
			while(packets[i][j] == '\0') {
				j--;
			}
			fileStream.write(packets[i] + sizeof(int) + sizeof(numPackets), j - sizeof(int) - sizeof(int) + 1);
		} else {
			fileStream.write(packets[i] + sizeof(int) + sizeof(numPackets), maxDataChunkSize);
		}
	}
	fileStream.close();
}

void FileManager::createPackets() {
	int seqNum;
	int maxDataChunkSize = MAX_PACKET_SIZE - sizeof(seqNum) - sizeof(numPackets);
    size_t fileSize;

	ifstream downloadStream;
	downloadStream.open(filename);

	if(downloadStream.bad()){
		cerr<<"Error: Failed to open downloaded file."<<endl;
	}
    filebuf* pbuf = downloadStream.rdbuf();
    fileSize = pbuf->pubseekoff (0,downloadStream.end,downloadStream.in);
    pbuf->pubseekpos (0,downloadStream.in);

    numPackets = ceil(double(fileSize)/double(maxDataChunkSize));
    packets = (char **)malloc(sizeof(char**)*numPackets);

    cout << "file size: " << fileSize << endl;
    cout << "packet size: " << MAX_PACKET_SIZE << endl;
    cout << "amount of date per packet: " << maxDataChunkSize << endl;
    cout << "number of packets required: " << numPackets << endl;
	
    for(int i = 0; i < numPackets; i++) {
    	seqNum = i;
    	int netSeqNum = htonl(seqNum);
    	int netNumPack = htonl(numPackets);
    	char *currentPacket = (char *)malloc(MAX_PACKET_SIZE);
    	memcpy(currentPacket, &netSeqNum, sizeof(netSeqNum));
    	memcpy(currentPacket + sizeof(netSeqNum), &netNumPack, sizeof(netNumPack));
    	downloadStream.read(currentPacket + sizeof(netSeqNum) + sizeof(netNumPack), maxDataChunkSize);
    	packets[i] = currentPacket;
    }
    string command = "rm " + filename;
    system(command.c_str());
}

void FileManager::printPackets() {
	for(int i = 0; i < numPackets; i++) {
		cout << "Packet " << i << " of " << numPackets << endl;
	}
}
// packet format: number of packets, seq number
void FileManager::acceptPacket(char *packet) {
	int seqNum;

	memcpy(&seqNum, packet, sizeof(seqNum));
	seqNum = ntohl(seqNum);
	if(numPackets == -1) {
		memcpy(&numPackets, packet + sizeof(seqNum), sizeof(numPackets));
		numPackets = ntohl(numPackets);
		packets = (char **)malloc(sizeof(char**)*numPackets);
	}

	packets[seqNum] = (char *)malloc(MAX_PACKET_SIZE);
	memcpy(packets[seqNum], packet, MAX_PACKET_SIZE);
}

// int main() {
// 	FileManager from;
// 	from.fetch("http://www.google.com");
// 	from.createPackets();
//
// 	FileManager to;
//
// 	int a[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
// 	random_shuffle(&a[0], &a[11]);
//
// 	for(int i = 0; i < from.numPackets; i++) {
// 		int packetToSend = a[i];
// 		to.acceptPacket(from.packets[packetToSend]);
// 	}
// 	to.download("testOutput");
// 	return 0;
// }
