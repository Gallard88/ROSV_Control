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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>

#include "TcpServer.h"

#define MAX_FP(a, b)  b = (a > b)? a : b

TcpServer::TcpServer(int port)
{
  struct sockaddr_in serv_addr;

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    exit(-1);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    exit(-1);
  }
  listen( listen_fd, 1);
}

TcpServer::~TcpServer(void)
{
  shutdown(listen_fd, 2);
  close(listen_fd);
  Clients.erase(Clients.begin(), Clients.end());
}

/* ======================== */
ClientSocket::Client_Ptr TcpServer::Listen(struct timeval timeout)
{
  ClientSocket::Client_Ptr ptr = NULL;
  fd_set readfs;
  int fp, new_fp = -1;

  // start by pruning old dead clients.
  for ( size_t i = 0; i < Clients.size(); i ++ ) {
    if ( Clients[i]->isConnected() == false ) {
      Clients.erase(Clients.begin() + i );
      break;
    }
  }

  FD_ZERO(&readfs);
  FD_SET(listen_fd, &readfs);
  int max_fp = listen_fd;

for ( auto& c: Clients ) {
    if ( c->isConnected() == true ) {
      fp = c->getFp();
      FD_SET(fp, &readfs);
      MAX_FP(fp, max_fp);
    }
  }

  if ( select(max_fp+1, &readfs, NULL, NULL, &timeout) > 0 ) {
    if ( FD_ISSET(listen_fd, &readfs)) {
      socklen_t clilen = sizeof(cli_addr);
      new_fp = accept(listen_fd, (struct sockaddr *) &cli_addr, &clilen);

      if ( new_fp >= 0 ) {
        ClientSocket::Client_Ptr p(new ClientSocket(std::string(inet_ntoa(cli_addr.sin_addr)), new_fp));
        Clients.push_back(p);
        ptr = p;
      }
    }
for ( auto& c: Clients ) {
      if ( c->isConnected() == true ) {
        fp = c->getFp();
        if ( FD_ISSET(fp, &readfs)) {
          c->ReadData();
        }
      }
    }
  }

  return ptr;
}

/* ======================== */
/* ======================== */
