#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h> 

using namespace std;

void receive(int fd)
{
    int len;
    char buf[BUFSIZ];
    char l[BUFSIZ];
    while(1)
    {
        printf("\n");
        len=recv(fd,buf,BUFSIZ,0);//接收服务器端信息
        buf[len]='\0';
        strcpy(l, buf);
        //printf("%s\n", l);
        //cout << len << " " << strcmp(l, "F") << endl;
        if(strcmp(l, "M") == 0)
        {
            len=recv(fd,buf,BUFSIZ,0);//接收服务器端信息
            buf[len]='\0';
            printf("%s",buf); 
            len=recv(fd,buf,BUFSIZ,0);//接收服务器端信息
            buf[len]='\0';
            printf("%s",buf); 
            len=recv(fd,buf,BUFSIZ,0);//接收服务器端信息
            buf[len]='\0';
            printf("%s",buf); 
        }
        else if(strcmp(l, "F") == 0)
        {
            len=recv(fd,buf,BUFSIZ,0);//SOURCE
            buf[len]='\0';
            printf("%s",buf); 
            len=recv(fd,buf,BUFSIZ,0);//:
            buf[len]='\0';
            printf("%s",buf); 
            len=recv(fd,buf,BUFSIZ,0);//FILE NAME
            buf[len]='\0';
            printf("%s",buf); 
            //printf("FILENAME : %s \n",buf); 
            ofstream out("recv/" + string(buf), ios::binary);

            len=recv(fd,buf,BUFSIZ,0);//LENGTH
            buf[len] = '\0';
            int num = atoi(buf);
            //printf("LEN : %d %d \n",len, num); 
            for(int i = 0; i < num; ++i)
            {
                len = recv(fd, buf, BUFSIZ, 0);
                buf[len] = '\0';
                out.write(buf, len);
            }
            out.close();
        }
        
        else printf("%s\n",l); 

    }
}


int main(int argc, char *argv[])  
{  
    

    int client_sockfd;  
    int len;  
    struct sockaddr_in remote_addr; 
    char buf[BUFSIZ];  
    memset(&remote_addr,0,sizeof(remote_addr)); 
    remote_addr.sin_family=AF_INET;
    remote_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    remote_addr.sin_port=htons(8002);
       
    if((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)  
    {  
        perror("socket");  
        return 1;  
    } 

    if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)  
    {  
        perror("connect");  
        return 1;  
    }  

    printf("connected to server\n");  
    len=recv(client_sockfd,buf,BUFSIZ,0);//接收服务器端信息  
    buf[len]='\0';  
    printf("%s",buf); //打印服务器端信息  
    
    while(1) //login info
    {
        cin >> buf;
        if(strcmp(buf, "register") == 0)
        {
            send(client_sockfd, "0", 1, 0);
            cout << "Please enter the username: ";
            cin >> buf;
            send(client_sockfd, buf, strlen(buf), 0);
            cout << "Please enter the password: ";
            cin >> buf;
            send(client_sockfd, buf, strlen(buf), 0);
            len = recv(client_sockfd,buf,BUFSIZ,0);
            buf[len]='\0'; 
            printf("%s\n",buf);
        }
        else if(strcmp(buf, "login") == 0)
        {  
            send(client_sockfd, "1", 1, 0);
            cout << "Please enter the username: ";
            cin >> buf;
            send(client_sockfd, buf, strlen(buf), 0);
            cout << "Please enter the password: ";
            cin >> buf;
            send(client_sockfd, buf, strlen(buf), 0);
            len = recv(client_sockfd,buf,BUFSIZ,0);
            buf[len]='\0'; 
            printf("%s\n",buf);
            if(strcmp(buf, "Login successfully!") == 0) break;
        }
        else
        {
            cout << "Invalid operation!\n";
        }
    }

    thread t(receive, client_sockfd);
    t.detach();
    while(1) //send and recv msg
    {
        cin >> buf;
        if(strcmp(buf, "add") == 0)
        {
            send(client_sockfd, "4", 1, 0);
            cout << "Enter the friend you want to add: ";
            cin >> buf;
            send(client_sockfd, buf, strlen(buf), 0);
            // len = recv(client_sockfd,buf,BUFSIZ,0);
            // buf[len]='\0'; 
            // printf("%s\n",buf);
        }
        else if(strcmp(buf, "remove") == 0)
        {
            send(client_sockfd, "5", 1, 0);
            cout << "Enter the friend you want to remove: ";
            cin >> buf;
            send(client_sockfd, buf, strlen(buf), 0);
            // len = recv(client_sockfd,buf,BUFSIZ,0);
            // buf[len]='\0'; 
            // printf("%s\n",buf);
        }
        else if(strcmp(buf, "sendmsg") == 0)
        {
            send(client_sockfd, "2", 1, 0);
            //cout << "Enter the friend you want to remove: ";
            
            cin >> buf;
            send(client_sockfd, buf, strlen(buf), 0);
            cin >> buf;
            send(client_sockfd, buf, strlen(buf), 0);
            // len = recv(client_sockfd,buf,BUFSIZ,0);
            // buf[len]='\0'; 
            // printf("%s\n",buf);
        }
        else if(strcmp(buf, "sendfile") == 0)
        {
            send(client_sockfd, "3", 1, 0);
            cin >> buf;
            send(client_sockfd, buf, strlen(buf), 0);
            cin >> buf;
            send(client_sockfd, buf, strlen(buf), 0);
            ifstream in("send/" + string(buf), ios::binary);
            if(!in.is_open())
            {
                printf("No such file!\n");
                continue;
            }

            in.seekg(0,ios::end);
            int length = in.tellg();
            int num = length / BUFSIZ + 1;
            char* numc = new char;
            sprintf(numc, "%d", num);
            sleep(0.02);
            send(client_sockfd, numc, sizeof(numc), 0);
            //cout << length << ' ' << numc <<  endl;
            in.seekg(0, ios::beg);
            for(int i = 0; i < num; ++i)
            {
                if(i < num - 1)
                {
                    in.read(buf, BUFSIZ);
                    sleep(0.02);
                    send(client_sockfd, buf, BUFSIZ, 0);
                }
                else 
                {
                    in.read(buf, length % BUFSIZ);
                    sleep(0.02);
                    send(client_sockfd, buf, length % BUFSIZ, 0);
                }
                //cout << length % BUFSIZ <<  endl;  
            }
        }
        else if(strcmp(buf, "friendlist") == 0)
        {
            send(client_sockfd, "6", 1, 0);
        }
    }
    
    close(client_sockfd);//关闭套接字  
    return 0;

}  