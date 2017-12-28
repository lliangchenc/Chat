CC			:= gcc
CXX			:= g++

LDFLAGS			:= -fpermissive -pthread -std=c++11 
all: server client

server: db.h server.h server.cpp
	$(CXX) $(LDFLAGS) -o $@ $?
client: client.cpp
	$(CXX) $(LDFLAGS) -o $@ $?
clean:
	rm -f server client
