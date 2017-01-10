#ifndef FILE_MAN_P2_H
#define FILE_MAN_P2_H

#include <iostream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <cmath>
#include <algorithm>

#define MAX_PACKET_SIZE 1000

using namespace std;

class FileManager {
public:
    string filename;
    int numPackets;
    char **packets;
    FileManager();
    void fetch(string url);
    void createPackets();
    void printPackets();
    void acceptPacket(char *packet);
    void download(string filename);


};

#endif