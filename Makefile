SRCS   			= ReqPacket.cpp FileManager.cpp
HEADERS 		= ReqPacket.h FileManager.h
AWGET_SRC		= awget.cpp
SS_SRC			= ss.cpp
AWGETEXE       		= awget
SSEXE			= ss
	
CC        		= g++
CC_FLAGS  		= -Wall -std=c++11 -O2

################################################################################

# Linking step
all: $(SRCS) $(HEADERS) $(SS_SRC) $(AWGET_SRC)
	$(CC) $(CC_FLAGS) $(SRCS) $(AWGET_SRC) -o $(AWGETEXE)
	$(CC) $(CC_FLAGS) $(SRCS) $(SS_SRC) -o $(SSEXE)

ss: $(SRCS) $(HEADERS) $(SS_SRC)
	$(CC) $(CC_FLAGS) $(SRCS) $(SS_SRC) -o $(SSEXE)

awget: $(SRCS) $(HEADERS) $(AWGET_SRC)
	$(CC) $(CC_FLAGS) $(SRCS) $(AWGET_SRC) -o $(AWGETEXE)

# Clean the directory in order to later build from scratch
clean:
	rm -rf *.o $(EXE)
