#include <iostream>
#include <fstream>
#include <cstring>
class Database
{
private:
	char** username;
	char** password;
	int user_num;
public:
	Database()
	{
		username = new char*[1000];
		password = new char*[1000];

	}
	int Load(char* filename);
	int Save(char* filename);
	int Lookup(char* username);
	int Add(char* u, char* p);
	char* getUserName(int No)
	{
		return username[No];
	}
	char* getPassWord(int No)
	{
		return password[No];
	}
};