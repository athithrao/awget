#include "ReqPacket.h"
#include "FileManager.h"

using namespace std;

#define MAXSSCLIENTS 10

class ss
{
				
    public:
    		reqPacket packet;
            FileManager *file;
    			
            bool ssrun(int port);
            void sswget(int socket);
            int toSSConnect();
            void printPacket();
            void findOutputName(string& filename);
    private:

    std::string getAddress(std::string port);
};

