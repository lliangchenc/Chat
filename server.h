#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cstring>
#include <thread>
#include <mutex>
#include <chrono>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>

#include "db.h"
//#include <mongoc.h>
using namespace std;

struct User
{
	int fd;
	char* username;
	char* password;
	vector<char*> friendlist;
};

struct Message
{
	char* source;
	char* target;
	char* content;
	int is_file;
};

class Chat_server
{
public:
	void Run();
	Chat_server();
private:
	//Database reg_list;
	vector<User> users;
	vector<User> online_users;
	vector<Message> messages;

	mutex user_mutex;
	mutex online_mutex;
	mutex msg_mutex;
	void Save();
	void sendMessage(int fd, char* usr);
	void recvMessage(int fd);
};

#endif