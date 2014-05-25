/*
 SubProtocol ( http://www.github.com/Gallard88/ROSV_Control )
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
// *******************************************************************************************
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sstream>
#include <syslog.h>
#include <ctype.h>

#include "parson.h"
#include "SubProtocol.h"

// *******************************************************************************************
using namespace std;

// *******************************************************************************************
SubProtocol::SubProtocol()
{
  update = 0;
}

// *******************************************************************************************
SubProtocol::~SubProtocol()
{
}

// *******************************************************************************************
void SubProtocol::AddModule(const string & name, CmdModule *mod)
{
  struct Modules new_module;

  new_module.Name = name;
  new_module.module = mod;
  Modules.push_back(new_module);
}

// *******************************************************************************************
void SubProtocol::SendSourceData(void)
{
  string msg;
  DataSource *src;

  msg = "{ \"Module\": \"ClientList\", ";
  msg += "\"RecordType\":\"Update\", ";
  msg += "\"DataType\":\"Text\", ";
  msg += "\"Values\":[ ";
  for ( size_t i = 0; i < Sources.size(); i ++ ) {
    src = Sources[i];
    msg += "\"";
    msg += string(src->GetName());
    msg += "\"";

    if ( Sources.size() > 1 ) {
      msg += ", ";
    }
  }
  msg += " ]}\r\n";
  SendMsg(&msg);
}

// *******************************************************************************************
void SubProtocol::AddSource(DataSource *src)
{
  Sources.push_back(src);

  for ( size_t i = 0; i < Modules.size(); i ++ ) {
    string msg;
    msg = "{ \"Module\": \"" + Modules[i].Name + "\", ";
    msg += "\"RecordType\":\"Config\", ";
    msg += Modules[i].module->GetConfigData();
    msg += " }\r\n";
    src->WriteData(msg.c_str());
  }
}

// *******************************************************************************************
void SubProtocol::ProcessLine(string line)
{
  printf("Line: %s\r\n", line.c_str());
  JSON_Value *data = json_parse_string(line.c_str());

  if ( data == NULL ) {
    printf("Data == NULL\n");
    return;
  }

  JSON_Object *obj = json_value_get_object(data);

  const char *module = json_object_get_string(obj, "Module");
  if ( module != NULL ) {
    for ( size_t j = 0; j < Modules.size(); j ++ ) {
      if ( strcmp(module, Modules[j].Name.c_str()) == 0 ) {
        Modules[j].module->Update(obj);
      }
    }
  }

  // release the data now that we are finished.
  json_value_free(data);

}

// *******************************************************************************************
#define MAX_FP(a, b)	b = (a > b)? a : b
// *******************************************************************************************
void SubProtocol::Run(struct timeval timeout)
{
  DataSource *src;
  fd_set readfs;
  int fd, max_fp = 0;
  size_t i;

  FD_ZERO(&readfs);
  for ( size_t i = 0; i < Sources.size(); i ++ ) {
    src = Sources[i];

    fd = src->GetFd();
    FD_SET(fd, &readfs);
    MAX_FP(fd, max_fp);
  }

  try {
    if ( select(max_fp+1, &readfs, NULL, NULL, &timeout) > 0 ) {
      for ( i = 0; i < Sources.size(); i ++ ) {
        src = Sources[i];
        if ( FD_ISSET(src->GetFd(), &readfs)) {
          src->ReadData();
        }
      }
    }
  } catch (int e) {
    src = Sources[i];
    Sources.erase(Sources.begin()+i);
    delete src;
  }

  for ( i = 0; i < Sources.size(); i ++ ) {
    string line;
    src = Sources[i];
    if ( src->ReadLine(&line)) {
      ProcessLine(line);
    }
  }

  /*
   * Now, grab the data from each module,
   * and send it to each DataSouce.
   */
  time_t current;
  current = time(NULL);
  if ((current - update) >= 1 ) {
    update = current;
    for ( size_t j = 0; j < Modules.size(); j ++ ) {
      string msg;

      msg = "{ \"Module\":\"" + Modules[j].Name + "\", ";
      msg += Modules[j].module->GetData();
      msg += " }\r\n";
      SendMsg(&msg);
    }
    SendSourceData();
  }
}

//*******************************************************************************************
void SubProtocol::SendMsg(const string *msg)
{
  size_t i;
  DataSource *src;

  try {
    for ( i = 0; i < Sources.size(); i ++ ) {
      src = Sources[i];
      src->WriteData(msg->c_str());
    }

  } catch (int e) {
    src = Sources[i];
    Sources.erase(Sources.begin()+i);
    delete src;
  }
}

//*******************************************************************************************
int SubProtocol::GetNumClients(void)
{
  return Sources.size();
}

//*******************************************************************************************
//*******************************************************************************************

