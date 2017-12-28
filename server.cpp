#include "server.h"

using namespace std;

int main()
{
	Chat_server chat;
	chat.Run();
	return 0;
}

Chat_server::Chat_server()
{
		
}

void Chat_server::recvMessage(int fd)
{
	char buf[BUFSIZ];
	char current_usrname[100];
	int len;
	int op;
	char username[100];
	char password[100];
	while(1)
	{
		
		len=recv(fd,buf,BUFSIZ,0); 
	    buf[len]='\0';
	    op = atoi(buf);  

		if(op == 0) //register
		{
			len=recv(fd,buf,BUFSIZ,0); //read username
	        buf[len]='\0'; 
	        printf("%s\n",buf);
	        strcpy(username, buf);

		    len=recv(fd,buf,BUFSIZ,0); //read password
	        buf[len]='\0';
	        printf("%s\n",buf);
	        strcpy(password, buf);
	        printf("%s %s\n",username, password);

		    
			int flag = 0;
			for(int i = 0; i < users.size(); ++i)
			{
				cout << "A " << users.at(i).username << " " << username << endl;
				if(strcmp(users.at(i).username, username) == 0)
				{
					char msg[100] = "Username already used!";
					send(fd, msg, strlen(msg), 0);
					flag = 1;
					break;
				}
			}
			if(flag == 0) //add a new user
			{
				User tmp;
				tmp.username = new char[100];
				tmp.password = new char[100];
				strcpy(tmp.username, username);
				strcpy(tmp.password, password);
				users.push_back(tmp);
				printf("size : %d\n", users.size());
				for(int i = 0; i < users.size(); ++i)
				{
					printf("USER : %d %s\n", i, users.at(i).username);
				}
				Save();
				char msg[100] = "Register successfully!";
				send(fd, msg, strlen(msg), 0);
			}
		}
		else if(op == 1) //login
		{

			len=recv(fd,buf,BUFSIZ,0); //read username
	        buf[len]='\0'; 
	        printf("%s\n",buf);
	        strcpy(username, buf);

	        len=recv(fd,buf,BUFSIZ,0); //read password
	        buf[len]='\0';
	        printf("%s\n",buf);
	        strcpy(password, buf);
	        printf("%s %s\n",username, password);


			int flag = 0;

			for(int i = 0; i < users.size(); ++i)
			{

				if(strcmp(users.at(i).username, username) == 0)
				{
					
					if(strcmp(users.at(i).password, password) == 0)
					{
						
						User tmp;
						tmp.fd = fd;
						tmp.username = new char[100];
						tmp.password = new char[100];
						strcpy(tmp.username, username);
						strcpy(tmp.password, password);
						//cout << "TEST HERE "  << ' ' << password << endl;
						tmp.friendlist.assign(users.at(i).friendlist.begin(), users.at(i).friendlist.end());
						online_users.push_back(tmp);
						char msg[100] = "Login successfully!";
						send(fd, msg, strlen(msg), 0);
						flag = 2;
					}
					else
					{
						char msg[100] = "Wrong password!";
						send(fd, msg, strlen(msg), 0);
						flag = 1;
					}
					break;
				}
			}
			if(flag == 0)
			{
				char msg[100] = "No such user!";
				send(fd, msg, strlen(msg), 0);
			}

			if(flag == 2) break;
		}
	}

	thread* send_t = new thread(&Chat_server::sendMessage, this, fd, username);
	send_t->detach();

	// recv msg N file
	char source[100];
	char target[100];
	char content[BUFSIZ];

	while(1)
	{
		len = recv(fd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		int op = atoi(buf);
		printf("%d\n", op);
		if(op == 2) //recv msg
		{
			strcpy(source, username);

			len = recv(fd, buf, BUFSIZ, 0);
			buf[len] = '\0';
			strcpy(target, buf);
			printf("%s %s\n", target, source);

			len = recv(fd, buf, BUFSIZ, 0);
			buf[len] = '\0';
			strcpy(content, buf);
			printf("%s\n", content);

			int flag = 0;
			for(int i = 0; i < users.size(); ++i)
			{
				if(strcmp(source, users.at(i).username) == 0)
				{
					for(int j = 0; j < users.at(i).friendlist.size(); ++j)
					{
						if(strcmp(target, users.at(i).friendlist.at(j)) == 0)
						{
							flag = 1;
							break;
						}
					}
					
				}
			}
			printf("%d %d\n", users.size(), flag);

			if(flag == 1)
			{
				Message m;
				m.source = new char[100];
				m.target = new char[100];
				m.content = new char[BUFSIZ];
				m.is_file = 0;
				strcpy(m.source, source);
				strcpy(m.target, target);
				strcpy(m.content, content);
				msg_mutex.try_lock();
				messages.push_back(m);
				msg_mutex.unlock();
				char msg[100] = "Send message successfully!\n";
				send(fd, msg, strlen(msg), 0);
			}
			else
			{
				char msg[100] = "No such user or such user is not your friend!\n";
				send(fd, msg, strlen(msg), 0);
			}
			
		}
		else if(op == 3) //recv file
		{
			strcpy(source, username);

			len = recv(fd, buf, BUFSIZ, 0);
			buf[len] = '\0';
			strcpy(target, buf);
			printf("%s %s\n", target, source);

			len = recv(fd, buf, BUFSIZ, 0);
			buf[len] = '\0';
			strcpy(content, buf);
			printf("%s\n", content);

			Message m;
			m.source = new char[100];
			m.target = new char[100];
			m.content = new char[BUFSIZ];
			m.is_file = 1;
			strcpy(m.source, source);
			strcpy(m.target, target);
			strcpy(m.content, content);

			cout << source << ' ' << target << ' ' << content << endl;

			int flag = 0;

			for(int i = 0; i < users.size(); ++i)
			{
				if(strcmp(source, users.at(i).username) == 0)
				{
					for(int j = 0; j < users.at(i).friendlist.size(); ++j)
					{
						if(strcmp(target, users.at(i).friendlist.at(j)) == 0)
						{
							flag = 1;
							break;
						}
					}
					
				}
			}

			printf("%d %d\n", users.size(), flag);

			if(flag == 1)
			{
				len = recv(fd, buf, BUFSIZ, 0);
				buf[len] = '\0';
				int num = atoi(buf);
				ofstream out("temp/" + string(content), ios::binary);

				for(int i = 0; i < num; ++i)
	            {
	                len = recv(fd, buf, BUFSIZ, 0);
	                buf[len] = '\0';
	                out.write(buf, len);
	                //cout << buf;
	            }
	            out.close();

	            
				msg_mutex.try_lock();
				messages.push_back(m);
				msg_mutex.unlock();

				char msg[100] = "Send file successfully!\n";
				send(fd, msg, strlen(msg), 0);
			}
			else
			{
				char msg[100] = "No such user or such user is not your friend!\n";
				send(fd, msg, strlen(msg), 0);
			}

			
		}
		else if(op == 4) //add friend
		{
			char* friendname = new char[100];
			len = recv(fd, buf, BUFSIZ, 0);
			buf[len] = '\0';
			strcpy(friendname, buf);
			printf("%s %s \n", friendname, username);
			int flag = 0;
			for(int i = 0; i < users.size(); ++i)
			{
				cout << "ADD " << users.at(i).username << " " << username << endl;
				if(strcmp(users.at(i).username, username) == 0)
				{
					int s = 0;
					int t = 0;
					for(int k = 0; k < users.size(); ++k)
					{
						//cout << users.at(k).username << " " << friendname << endl;
						if(strcmp(users.at(k).username, friendname) == 0)
						{
							t = 1;
							break;
						}
					}
					for(int j = 0; j < users.at(i).friendlist.size(); ++j)
					{
						if(strcmp(users.at(i).friendlist.at(j), friendname) == 0)
						{
							s = 1;
							break;
						}
					}
					//cout << "FRIEND " << s << " " << t << endl;
					if(s == 0 && t == 1)
					{
						users.at(i).friendlist.push_back(friendname);
						flag = 1;
					}
					
					printf("%d\n", users.at(i).friendlist.size());
					for(int j = 0; j < users.at(i).friendlist.size(); ++j)
					{
						printf("%d %s\n", j, users.at(i).friendlist.at(j));
					}
					break;
				}
			}
			printf("AAAA");
			if(flag == 1)
			{
				Save();
				char msg[100] = "Add friend successfully!\n";
				send(fd, msg, strlen(msg), 0);
			}
			else
			{
				char msg[100] = "No such user or it's already your friend!";
				send(fd, msg, strlen(msg), 0);
			}
		}
		else if(op == 5) //remove friend
		{

		}
		else if(op == 6) //view friendlist
		{
			for(int i = 0; i < users.size(); ++i)
			{
				if(strcmp(username, users.at(i).username) == 0)
				{
					//sleep(0.01);
		            //send(fd, "L", 1, 0);
		            //sleep(0.02);
		            //char* numc = new char;
		            char s[BUFSIZ];
		            //sprintf(numc, "%d", users.at(i).friendlist.size());
		            //send(fd, numc, sizeof(numc), 0);
		            strcpy(s, "");
		            for(int j = 0; j < users.at(i).friendlist.size(); ++j)
		            {
		            	
		            	strcat(s, users.at(i).friendlist.at(j));
		            	strcat(s, (char*)" ");
		            	//strcpy(numc, s);
		            	//sleep(0.01);
		            	//send(fd, users.at(i).friendlist.at(j), sizeof(users.at(i).friendlist.at(j)), 0);
		            }
		            send(fd, s, sizeof(s), 0);
				}
			}
		}
	}
}

void Chat_server::sendMessage(int fd, char* usr)
{
	char temp[BUFSIZ];
	char buf[BUFSIZ];
	while(1)
	{
		//printf("HEHE %d\n", messages.size());
		sleep(0.2);
		msg_mutex.try_lock();
		for(int i = 0; i < messages.size(); ++i)
		{
			if(strcmp(usr, messages.at(i).target) == 0)
			{
				if(messages.at(i).is_file == 0)
				{
					printf("SEND %s %s\n", messages.at(i).source, messages.at(i).content);
					sleep(0.01);
					send(fd, "M", 1, 0);
					//sleep(0.1);
					sleep(0.02);
					send(fd, messages.at(i).source, strlen(messages.at(i).source), 0);
					sleep(0.02);
					send(fd, " : \n  ", 6, 0);
					sleep(0.02);
					send(fd, messages.at(i).content, strlen(messages.at(i).content), 0);
					messages.erase(messages.begin() + i);
				}
				else
				{
					printf("SENDFILE %s %s\n", messages.at(i).source, messages.at(i).content);
					sleep(0.01);
					send(fd, "F", 1, 0);
					sleep(0.02);
					send(fd, messages.at(i).source, strlen(messages.at(i).source), 0);
					sleep(0.02);
					send(fd, " send you a file : \n  ", 22, 0);
					sleep(0.02);
					send(fd, messages.at(i).content, strlen(messages.at(i).content), 0);
					sleep(0.02);
					ifstream in("temp/" + string(messages.at(i).content), ios::binary);
		            in.seekg(0,ios::end);
		            int length = in.tellg();
		            int num = length / BUFSIZ + 1;
		            char* numc = new char;
		            sprintf(numc, "%d", num);
		            send(fd, numc, sizeof(numc), 0);
		            cout << length << ' ' << numc <<  endl;
		            in.seekg(0, ios::beg);
		            for(int i = 0; i < num; ++i)
		            {
		            	cout << i <<  endl;
		                if(i < num - 1)
		                {
		                    in.read(buf, BUFSIZ);
		                    send(fd, buf, BUFSIZ, 0);
		                }
		                else 
		                {
		                    in.read(buf, length % BUFSIZ);
		                    send(fd, buf, length % BUFSIZ, 0);
		                }
		            }

					// send(fd, "0", 1, 0);
					// send(fd, messages.at(i).source, strlen(messages.at(i).source), 0);
					// send(fd, " : \n  ", 6, 0);
					// send(fd, messages.at(i).content, strlen(messages.at(i).content), 0);
					messages.erase(messages.begin() + i);
				}
				
			}
		}

		
		msg_mutex.unlock();
	}
}
void Chat_server::Save()
{
	ofstream fout("register_list.txt");
	for(int i = 0; i < users.size(); ++i)
	{
		fout << users.at(i).username << " " << users.at(i).password << " " << users.at(i).friendlist.size();
		for(int j = 0; j < users.at(i).friendlist.size(); ++j)
		{
			fout << " " << users.at(i).friendlist.at(j);
		}
		fout << endl;
	}
	fout.close();
}

void Chat_server::Run()
{
	int server_fd;
	int client_fd[50];
	int sin_size;
	int len;
	int friend_num;
	char temp[100];
	users.clear();
	ifstream fin("register_list.txt");
	while(fin >> temp)
	{
		cout << temp << endl;
		User u;
		u.username = new char[100];
		u.password = new char[100];
		strcpy(u.username, temp);
		fin >> temp;
		strcpy(u.password, temp);
		fin >> friend_num;
		for(int i = 0; i < friend_num; ++i)
		{
			fin >> temp;
			char* f = new char[100];
			strcpy(f, temp);
			u.friendlist.push_back(f);
		}
		//users.insert(users.begin() + users.size(), u);
		users.push_back(u);
	}
	fin.close();

	thread* recv_t[50];
	struct sockaddr_in my_addr;
	struct sockaddr_in remote_addr;
	memset(&my_addr,0,sizeof(my_addr)); 

	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = htons(8002); 

	if((server_fd=socket(PF_INET,SOCK_STREAM,0))<0)  
    {    
        perror("socket");  
        return;  
    }

    if (bind(server_fd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)  
    {  
        perror("bind");  
        return;  
    }  
	
	listen(server_fd, 50);

	//build connection
	
	sin_size=sizeof(struct sockaddr_in);  

	for(int i = 0; i < 50; ++i)
	{
		if((client_fd[i]=accept(server_fd,(struct sockaddr *)&remote_addr,(socklen_t*)&sin_size))<0)  
	    {  
	        perror("accept");  
	        return;  
	    }  
		//client_fd[i] = accept(server_fd, (struct sockaddr*)&remote_addr, (socklen_t*)&sin_size);
		printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));  
	    len=send(client_fd[i],"Welcome to my server\n",21,0);//发送欢迎信息  
		
		recv_t[i] = new thread(&Chat_server::recvMessage, this, client_fd[i]);
		recv_t[i]->detach();
		//pthread_join()
	}

}