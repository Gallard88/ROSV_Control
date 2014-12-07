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

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "TcpServer.h"

#define MAX_FP(a, b)  b = (a > b)? a : b

const int TcpServer_WriteEx = 1;
const int TcpServer_HandleNotFound = 2;


TcpServer::TcpServer(int port): NextFd(0)
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

for ( auto& c: Clients) {
    if ( c.second.File >= 0 ) {
      close(c.second.File);
      c.second.File = -1;
    }
  }
  Clients.clear();
}

/* ======================== */
int TcpServer::Listen(struct timeval timeout)
{
  fd_set readfs;
  int fp, new_fp = -1;

  // start by pruning old dead clients.
  CleanMap();

  FD_ZERO(&readfs);
  FD_SET(listen_fd, &readfs);
  int max_fp = listen_fd;

for ( auto& c: Clients) {
    fp = c.second.File;
    FD_SET(fp, &readfs);
    MAX_FP(fp, max_fp);
  }

  if ( select(max_fp+1, &readfs, NULL, NULL, &timeout) > 0 ) {
    if ( FD_ISSET(listen_fd, &readfs)) {
      socklen_t clilen = sizeof(cli_addr);
      new_fp = accept(listen_fd, (struct sockaddr *) &cli_addr, &clilen);

      if ( new_fp >= 0 ) {
        struct TcpClient client;
        client.Name = string(inet_ntoa(cli_addr.sin_addr));
        client.File = new_fp;
        Clients[NextFd] = client;
        new_fp = NextFd;
        NextFd++;
      }
    }
for ( auto& c: Clients) {
      fp = c.second.File;
      if ( FD_ISSET(fp, &readfs)) {
        char data[8192];
        int n = read(fp, data, sizeof(data)-1);

        if ( n <= 0 ) {
          close(c.second.File);
          c.second.File = -1;

        } else {
          data[n] = 0;
          c.second.Buffer += string(data);
        }
      }
    }
  }

  return new_fp;
}

/* ======================== */
/* ======================== */

void TcpServer::Write(int handle, const string & msg)
{
  if (( Clients.find(handle) == Clients.end()) ||
      ( Clients[handle].File < 0 )) {
    throw TcpServer_HandleNotFound;
  }

  int n = write(Clients[handle].File, msg.c_str(), msg.size());
  if ( n <= 0 ) {

    close(Clients[handle].File);
    Clients[handle].File = 1;
    throw TcpServer_WriteEx;
  }
}

bool TcpServer::Handle_ReadLine(int handle, string & line)
{
  if (( Clients.find(handle) == Clients.end()) ||
      ( Clients[handle].File < 0 )) {
    throw TcpServer_HandleNotFound;
  }

  while ( Clients[handle].Buffer.size() != 0 ) {
    size_t found = Clients[handle].Buffer.find_first_of("\r\n");

    if ( found != string::npos) {

      line = Clients[handle].Buffer.substr(0, found);
      Clients[handle].Buffer.erase(0, found+1);

      if ( line.length())
        return true;
    }
  }
  return false;
}

string TcpServer::GetHandleName(int handle)
{
  if ( Clients.find(handle) == Clients.end()) {
    return "";
  }
  return Clients[handle].Name;
}

void TcpServer::CleanMap(void)
{
for ( auto& c: Clients) {
    if ( c.second.File == -1 ) {
      c.second.Buffer.clear();
      Clients.erase(c.first);
      break;
    }
  }
}


/* ======================== */
/* ======================== */

