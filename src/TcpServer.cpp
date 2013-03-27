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
  return listen_fd;
}

/* ======================== */
void TcpServer::CheckNewConnetions(void)
{
  struct TcpData new_sock;
  socklen_t clilen;
  struct sockaddr_in cli_addr;

  if ( FD_ISSET(listen_fd, &readfs) )
  {
//		cout << "FD set" << endl;
    clilen = sizeof(cli_addr);
    new_sock.fd = accept(listen_fd, (struct sockaddr *) &cli_addr, &clilen);

    if (new_sock.fd < 0 )
      return ;

    Socket_Vec.push_back(new_sock);
  }
}

/* ======================== */
#define SET_MAX_SOCK(x)	if( x > max_sock ) max_sock = x;

/* ======================== */
void TcpServer::Run(struct timeval *timeout)
{
//	fd_set readfs;
  int max_sock = 0;
  struct TcpData new_sock;
  struct timeval to = { 0, 0};
  char buffer[4096];

  if ( timeout)
    to = *timeout;

  Prune();

  // reset Select Data.
  FD_ZERO(&readfs);
  if ( Socket_Vec.size() < (size_t) Max_Connections )
  {
    FD_SET(listen_fd, &readfs);
    max_sock = listen_fd;
  }

  for (std::vector<struct TcpData>::iterator it = Socket_Vec.begin() ; it != Socket_Vec.end(); ++it)
  {
    FD_SET(it->fd, &readfs);
    SET_MAX_SOCK( it->fd );
  }
  if ( select(max_sock+1, &readfs, NULL, NULL, &to) <= 0)
    return;

  if ( Socket_Vec.size() < (size_t) Max_Connections)
    CheckNewConnetions();

  for (std::vector<struct TcpData>::iterator it = Socket_Vec.begin() ; it != Socket_Vec.end(); ++it)
  {
    int sock = it->fd;
    if ( FD_ISSET(sock, &readfs) )
    {
      int n = read(sock, buffer, sizeof(buffer));
      if ( n <= 0 )
      {
        shutdown(it->fd, 2);
        it->fd = -1;
      }
      else if ( n > 0 )
      {
        string msg = string(buffer);
        it->buffer += msg;
      }
    }
  }
}

/* ======================== */
int TcpServer::ReadLine(string *data)
{
  for (std::vector<struct TcpData>::iterator it = Socket_Vec.begin() ; it != Socket_Vec.end(); ++it)
  {
    string *line = &it->buffer;
    if ( line->size() == 0 )
      continue;

    size_t found = line->find_first_of("\r\n");
    if ( found != string::npos)
    {
      *data = line->substr(0, found);
//			if ( data->size() > 0 )
//  			data->append("\r\n");
      line->erase(0, found+1);

      return 1;
    }
  }
  return 0;
}

/* ======================== */
void TcpServer::Prune(void)
{
  for (std::vector<struct TcpData>::iterator it = Socket_Vec.begin() ; it != Socket_Vec.end(); ++it)
  {
    if ( it->fd < 0 )
    {
      cout << "Client Lost" << endl;
      it->buffer.clear();
      Socket_Vec.erase(it);
      Prune(); // recursive to remove all closes sockets.
      // any other method seems to cause a seg fault on the next iteration of the for() loop
      return;
    }
  }
}

/* ======================== */
void TcpServer::SendMsg(const string msg)
{
  SendMsg((const char *)msg.c_str(), (int ) msg.size());
}

/* ======================== */
void TcpServer::SendMsg(const char *data, int size)
{
  int rv;

  for (std::vector<struct TcpData>::iterator it = Socket_Vec.begin() ; it != Socket_Vec.end(); ++it)
  {
    rv = write(it->fd, data, size);
    if (rv < 0 )
    {
      shutdown(it->fd, 2);
      it->fd = -1;
    }
  }
}

/* ======================== */
TcpServer::~TcpServer(void)
{
  for (std::vector<struct TcpData>::iterator it = Socket_Vec.begin() ; it != Socket_Vec.end(); ++it)
  {
    shutdown(it->fd, 2);
    close(it->fd);
    it->buffer.clear();
    it->fd = -1;
  }
  Socket_Vec.clear();
  shutdown(listen_fd, 2);
  close(listen_fd);
  listen_fd = -1;
}

/* ======================== */
/* ======================== */
