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
#include <cstring>
#include "parson.h"
#include "SubProtocol.h"

// *******************************************************************************************
using namespace std;

/*
 * The ROSV Protocol is a general purpose protocol, that enables the various
 * sub systems and controllers to communicate with the Sub.
 * As such there are messages for many modules that can be passed through the same connection.
 * Therefore a multiplexer system is needed to route the packets to the correct destination.
 * The general purpose mux packet is as follows:
 *
 * {
 *   "Module":"",
 *   "Packet":"",
 *   --- Packet Data ---
 * }\r\n
 *
 * There are some limitations, namemly there cannot be any \r\n with in the packet itself, as this is used to dennote end of line.
 *
 *
 */

// *******************************************************************************************
SubProtocol::SubProtocol()
{
  Server = new TcpServer(8090);
  AddModule("Clients", this);
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
  new_module.PTime = 0;
  new_module.module = mod;
  Modules.push_back(new_module);
}

// *******************************************************************************************
#define MAX_FP(a, b)  b = (a > b)? a : b
// *******************************************************************************************
void SubProtocol::Run(struct timeval timeout)
{
  int fd;
  size_t i;

  fd = Server->Listen(timeout);
  if ( fd >= 0 ) {
    if ( IntListeners != NULL ) {
      IntListeners->Client_Added(Server->GetHandleName(fd), fd);
    }
    Handles.push_back(fd);
    ResetPacketTime();
    PacketTime = time(NULL);
  }

  try {
    string l;
    for ( i = 0; i < Handles.size(); i ++ ) {
      while ( Server->Handle_ReadLine(Handles[i], l) == true ) {
        ProcessLine(l);
      }
    }
  } catch (int ex ) {
    if ( IntListeners != NULL ) {
      IntListeners->Client_Removed(Handles[i]);
    }
    Handles.erase (Handles.begin()+i);
    PacketTime = time(NULL);
  }

  if ( Handles.size() != 0 ) {
    SendUpdatedData();
  }
}

// *******************************************************************************************
void SubProtocol::ResetPacketTime(void)
{
for ( auto& m: Modules) {
    m.PTime = 0;
  }
}


// *******************************************************************************************
void SubProtocol::SendUpdatedData(void)
{
  /*
   * Now, grab the data from each module,
   * and send it to each DataSouce.
   */
  for ( size_t j = 0; j < Modules.size(); j ++ ) {
    if ( Modules[j].PTime != Modules[j].module->PacketTime ) {
      Modules[j].PTime = Modules[j].module->PacketTime;

      string msg;
      msg = "{ \"Module\":\"" + Modules[j].Name + "\", ";
      msg += Modules[j].module->GetData();
      msg += " }\r\n";
      SendMsg(msg);
    }
  }
}

//*******************************************************************************************
void SubProtocol::SendMsg(const string & msg)
{
  size_t i;

  try {

    for ( i = 0; i < Handles.size(); i ++ ) {
      Server->Write(Handles[i], msg);
    }

  } catch (int e) {
    if ( IntListeners != NULL ) {
      IntListeners->Client_Removed(Handles[i]);
    }
    PacketTime = time(NULL);
    Handles.erase(Handles.begin()+i);
  }
}

// *******************************************************************************************
void SubProtocol::Update(const char *packet, JSON_Object *msg)
{
}

// *******************************************************************************************
const string SubProtocol::GetData(void)
{
  string msg;

  msg = "{ \"Module\": \"ClientList\", ";
  msg += "\"RecordType\":\"Clients\", ";
  msg += "\"Values\":[ ";
  for ( size_t i = 0; i < Handles.size(); i ++ ) {
    msg += "{ \"ip\":\"";
    msg += string(Server->GetHandleName(Handles[i]));
    msg += "\"} ";

    if (( Handles.size() > 1 ) && ( i <  (Handles.size()-1))) {
      msg += ", ";
    }
  }
  msg += " ]}\r\n";
  return msg;
}

// *******************************************************************************************
void SubProtocol::ProcessLine(string line)
{
//  printf("Line: %s\r\n", line.c_str());
  JSON_Value *data = json_parse_string(line.c_str());

  if ( data == NULL ) {
    return;
  }

  JSON_Object *obj = json_value_get_object(data);

  const char *module = json_object_get_string(obj, "Module");
  const char *packet = json_object_get_string(obj, "Packet");
  if ( module != NULL ) {
    for ( size_t j = 0; j < Modules.size(); j ++ ) {
      if ( strcmp(module, Modules[j].Name.c_str()) == 0 ) {
        Modules[j].module->Update(packet, obj);
      }
    }
  }
  // release the data now that we are finished.
  json_value_free(data);
}

// *******************************************************************************************
void SubProtocol::AddListener(SubProt_Interface *listen)
{
  IntListeners = listen;
}

// *******************************************************************************************
int SubProtocol::GetNumClients(void)
{
  return Handles.size();
}

// *******************************************************************************************
// *******************************************************************************************

