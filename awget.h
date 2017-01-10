#include "ReqPacket.h"
#include "FileManager.h"

using namespace std;

class awget
{
				
    public:
    		reqPacket packet;
            FileManager *file;
    			
            bool readSSList(char* fileName);
            bool awgetrun();
            void printPacket();
    private:
        void findOutputName(string& filename);
};

