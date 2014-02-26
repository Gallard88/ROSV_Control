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

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    syslog(LOG_EMERG, "Socket() error, port %d", port);
    exit(-1);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    syslog(LOG_EMERG, "ERROR on binding to port %d", port);
    exit(-1);
  }
  syslog(LOG_INFO, "TcpServer setup, port: %d", port);
  listen( listen_fd, 1);
}

/* ======================== */
TcpServer::~TcpServer(void)
{
  shutdown(listen_fd, 2);
  close(listen_fd);
  listen_fd = -1;
}

/* ======================== */
int TcpServer::GetFp(void)
{
  return listen_fd;
}

/* ======================== */
DataSource *TcpServer::Listen(void)
{
  DataSource *src = NULL;
  socklen_t clilen;
  int fp;

  clilen = sizeof(cli_addr);
  fp = accept(listen_fd, (struct sockaddr *) &cli_addr, &clilen);
  if ( fp >= 0 ) {
    src = new DataSource(fp, inet_ntoa(cli_addr.sin_addr));
  }
  return src;
}

/* ======================== */
/* ======================== */

