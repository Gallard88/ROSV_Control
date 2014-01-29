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
#include <stdio.h>
#include <syslog.h>

using namespace std;

#include "DataSource.h"

//*******************************************************************************************
DataSource::DataSource(void)
{
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
  printf("New DataSource: %s\n", name);
//  syslog(LOG_EMERG, "New DataSource: %s", name);
}

//*******************************************************************************************
DataSource::~DataSource(void)
{

  syslog(LOG_EMERG, "Del DataSource, %d, %s", File, this->Name);
  printf("DataSource del , %d, %s\n", File, this->Name);
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
const int DS_WriteEx = 2;

//*******************************************************************************************
int DataSource::ReadData(void)
{
  char buffer[8192];
  string msg;

  int n = read(File, buffer, sizeof(buffer)-1);
  if ( n <= 0 ) {
		printf("Read Exception\n");
    throw DS_ReadEx;
    // throw expection.
  } else {
    buffer[n] = 0;
    msg.clear();
    msg = string(buffer);
    Buffer += msg;
  }
  return n;
}

//*******************************************************************************************
int DataSource::ReadLine(string *line)
{
  line->clear();
  if ( File >= 0 ) {
    while ( Buffer.size() != 0 ) {
      size_t found = Buffer.find_first_of("\r\n");
      if ( found != string::npos) {
        *line = Buffer.substr(0, found);
        Buffer.erase(0, found+1);
        if ( line->length())
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
  if ( n <= 0 ) {
		printf("Write Exception\n");
    throw DS_WriteEx;
  }
  return n;
}

//*******************************************************************************************
//*******************************************************************************************
