/*
 TcpServer ( http://www.github.com/Gallard88/ROSV_Control )
 Copyright (c) 2013 Thomas BURNS

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

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
#include <syslog.h>
#include <arpa/inet.h>

#include "TcpServer.h"

/* ======================== */
TcpServer::TcpServer(int port)
{
  struct sockaddr_in serv_addr;

	File = -1;

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0)
  {
		syslog(LOG_EMERG, "Socket() error");
    return;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
		syslog(LOG_EMERG, "ERROR on binding");
    return;
  }
  listen( listen_fd, 1);
  return;
}

/* ======================== */
void TcpServer::CheckReturn(int rv)
{
	if ( rv < 0 )
	{
		close(File);
		File = -1;
		Buffer.erase();
		syslog(LOG_EMERG, "Lost Connection: %s", inet_ntoa(cli_addr.sin_addr));
	}
}

/* ======================== */
int TcpServer::ReadLine(string *line)
{
	if ( File >= 0 )
  {
    if ( Buffer.size() != 0 )
		{
			size_t found = Buffer.find_first_of("\r\n");
			if ( found != string::npos)
			{
				*line = Buffer.substr(0, found);
				Buffer.erase(0, found+1);
				return 1;
			}
		}
  }
  return 0;
}

/* ======================== */
int TcpServer::WriteData(const char *msg, int length)
{
	int rv = 0;
	if ( File >= 0 )
	{
		rv = write(File, msg, length);
		CheckReturn(rv);
	}
	return rv;
}

/* ======================== */
int TcpServer::WriteData(const string & line)
{
	int rv = 0;
	if ( File >= 0 )
	{
		rv = write(File, line.c_str(), line.size());
		CheckReturn(rv);
	}
	return rv;
}

/* ======================== */
void TcpServer::Run(const struct timeval *timeout)
{
	int fd;
	fd_set readfs;
  char buffer[4096];

	if ( File < 0 ) // listen for new connections
	  fd = listen_fd;
	else // run file socket
		fd = File;

  FD_ZERO(&readfs);
  FD_SET(fd, &readfs);
	struct timeval to = *timeout;
  if ( select(fd+1, &readfs, NULL, NULL, &to) <= 0)
    return;

	if (( File < 0 ) && FD_ISSET(listen_fd, &readfs))
	{
		socklen_t clilen;

    clilen = sizeof(cli_addr);
    File = accept(listen_fd, (struct sockaddr *) &cli_addr, &clilen);
		syslog(LOG_EMERG, "New Connection: %s", inet_ntoa(cli_addr.sin_addr));
	}
	else  if (FD_ISSET(File, &readfs))
	{
		int n = read(File, buffer, sizeof(buffer));
		if ( n > 0 ) {
			string msg = string(buffer);
			Buffer += msg;
		}
		else {
			n = -1;
		}
		CheckReturn(n);
	}
}

/* ======================== */
/* ======================== */
TcpServer::~TcpServer(void)
{
	Buffer.clear();
	if ( File >= 0 )
	{
    shutdown(File, 2);
    close(File);
		File = -1;
	}
	syslog(LOG_EMERG, "Shutting down module");
  shutdown(listen_fd, 2);
  close(listen_fd);
  listen_fd = -1;
}

/* ======================== */
/* ======================== */
