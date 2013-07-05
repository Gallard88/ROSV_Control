/*
 DataSource ( http://www.github.com/Gallard88/ROSV_Control )
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
//*******************************************************************************************
//*******************************************************************************************
#include <string.h>

using namespace std;

#include "DataSource.h"

//*******************************************************************************************
DataSource::DataSource(void) {
    File = -1;
	Name = NULL;
  }

//*******************************************************************************************
DataSource::DataSource(int fp, const char *name)
{
  File = fp;

  int length = strlen(name);
  this->Name = new char[length+1];
  strncpy(this->Name, name, length);
  this->Name[length] = 0;

}

//*******************************************************************************************
DataSource::~DataSource(void) {
  if ( File >=0 ) {
    close(File);
    File = -1;
  }
  delete this->Name;
  this->Name = NULL;
}

//*******************************************************************************************
const char *DataSource::GetName(void)
{
  return this->Name;
}

//*******************************************************************************************
const int DS_ReadEx = 1;

//*******************************************************************************************
int DataSource::ReadData(void)
{
  char buffer[4096];

  int n = read(File, buffer, sizeof(buffer));
  if ( n < 0 ) {
	throw DS_ReadEx;
	// throw expection.
  } else {
    string msg = string(buffer);
    Buffer += msg;
  }
  return n;
}

//*******************************************************************************************
int DataSource::ReadLine(string *line)
{
  if ( File >= 0 ) {
    if ( Buffer.size() != 0 ) {
      size_t found = Buffer.find_first_of("\r\n");
      if ( found != string::npos) {
        *line = Buffer.substr(0, found);
        Buffer.erase(0, found+1);
        return line->length();
      }
    }
  }
  return 0;
}
//*******************************************************************************************
int DataSource::WriteData(const char *msg)
{
	int n = write (File, msg, strlen(msg));
//	if ( n <= 0 )
	return n;
}

//*******************************************************************************************
//*******************************************************************************************
/* ======================== */
#if 0
void TcpServer::CheckReturn(int rv)
{
  if ( rv < 0 ) {
    close(File);
    File = -1;
    Buffer.erase();
    syslog(LOG_EMERG, "Lost Connection: %s", inet_ntoa(cli_addr.sin_addr));
    printf("Lost Connection: %s\n", inet_ntoa(cli_addr.sin_addr));
  }
}

/* ======================== */
int TcpServer::WriteData(const char *msg, int length)
{
  int rv = 0;
  if ( File >= 0 ) {
    rv = write(File, msg, length);
    CheckReturn(rv);
  }
  return rv;
}

/* ======================== */
int TcpServer::WriteData(const string & line)
{
  int rv = 0;
  if ( File >= 0 ) {
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

  if (( File < 0 ) && FD_ISSET(listen_fd, &readfs)) {
    socklen_t clilen;

    clilen = sizeof(cli_addr);
    File = accept(listen_fd, (struct sockaddr *) &cli_addr, &clilen);
    syslog(LOG_EMERG, "New Connection: %s", inet_ntoa(cli_addr.sin_addr));
    printf("New Connection: %s\n", inet_ntoa(cli_addr.sin_addr));
  } else  if (FD_ISSET(File, &readfs)) {
    int n = read(File, buffer, sizeof(buffer));
    if ( n > 0 ) {
      string msg = string(buffer);
      Buffer += msg;
    } else {
      n = -1;
    }
    CheckReturn(n);
  }
}
#endif

