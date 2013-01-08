/* ======================== */
using namespace std;
/* ======================== */
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "TcpServer.h"

/* ======================== */
TcpServer::TcpServer(int max_clients)
{
	Max_Connections = max_clients;
//	poll = new struct pollfd [max_clients];
}

/* ======================== */
int TcpServer::Connect(int port)
{
	struct sockaddr_in serv_addr;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
	{
		printf("Socket() error");
		return -1;
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR on binding\n");
		return -1;
	}
	listen( listen_fd, Max_Connections);
//	BuildSelectList();

	return listen_fd;
}

/* ======================== */
void TcpServer::CheckNewConnetions(void)
{
	struct TcpData new_sock;
	socklen_t clilen;
	struct sockaddr_in cli_addr;

//	if ( FD_ISSET(listen_fd, &readfds) )
	{
		cout << "FD set" << endl;
		clilen = sizeof(cli_addr);
		new_sock.fd = accept(listen_fd, (struct sockaddr *) &cli_addr, &clilen);
		cout << "accept"<< endl;

		if (new_sock.fd < 0 )
		{
			printf("ERROR on accept");
			return;
		}
		Socket_Vec.push_back(new_sock);
	}
}

/* ======================== */
#define SET_MAX_SOCK(x)	if( x > max_sock ) max_sock = x;

/* ======================== */
void TcpServer::Run(struct timeval *timeout)
{
//	struct sockaddr_in cli_addr;
//	socklen_t clilen;
	fd_set readfs;
	int max_sock = 0;
	struct TcpData new_sock;
	struct timeval to = { 0, 0};
	char buffer[4096];


	if ( timeout)
		to = *timeout;

  FD_ZERO(&readfs);
	FD_SET(listen_fd, &readfs);
	max_sock = listen_fd;

	for ( int i = 0;  i < Socket_Vec.size(); i ++)
	{
		FD_SET(Socket_Vec[i].fd, &readfs);
		SET_MAX_SOCK( Socket_Vec[i].fd );
	}
	cout << "Select()" << endl;
	select(max_sock+1, &readfs, NULL, NULL, &to);

	cout << "Check Sockets" << endl;
	for (int i = 0; i < Socket_Vec.size(); i++ )
	{
		cout << "I: "<< i << endl;
		int sock = Socket_Vec[i].fd;
		if ( FD_ISSET(Socket_Vec[i].fd, &readfs) )
		{
			cout << "FD isset" << endl;
			int n = read(sock, buffer, sizeof(buffer));
			if ( n < 0 )
				cout << "Socket " << n << " Exception" << endl;
			else if ( n > 0 )
			{
				cout << "Bytes " << n << endl;
				string msg = string(buffer);
				Socket_Vec[i].buffer += msg;
			}
		}
	}
/*
	cout << "Check for new Clients" << endl;
	// check for new clients;
	if ( FD_ISSET(listen_fd, &readfs) )
	{
		cout << "FD set" << endl;
		clilen = sizeof(cli_addr);
		new_sock.fd = accept(listen_fd, (struct sockaddr *) &cli_addr, &clilen);
		cout << "accept"<< endl;

		if (new_sock.fd < 0 )
			printf("ERROR on accept");
		else
			Socket_Vec.push_back(new_sock);
	}
	*/
}

/* ======================== */
int TcpServer::ReadLine(string *cmd, string *arg)
{
	int found;
	for (int i = 0; i < Socket_Vec.size(); i++ )
	{
		string *line = &Socket_Vec[i].buffer;

		if ((found = line->find_first_of("\r")) != string::npos)
		{
			*cmd = line->substr(0, found);
			line->erase(0, found);
			cout << "Cmd found: " << *cmd << endl;

			size_t x = cmd->find_first_of("=:");
			if ( x != string::npos )
			{
				*arg = cmd->substr((size_t) x+1, cmd->size());
				cmd->erase((size_t)x, cmd->size());
				cout << "Arg Built: " << *arg << endl;
			}
			return 1;
		}
	}
	return 0;
}

/* ======================== */
/* ======================== */

#if 0
/* A simple server in the internet domain using TCP
   The port number is passed as an argument */


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);

     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");

		 bzero(buffer,256);

     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     printf("Here is the message: %s\n",buffer);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) error("ERROR writing to socket");
     close(newsockfd);
     close(sockfd);
     return 0;
}
#endif
