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
  std::shared_ptr<Permission> p(new Permission("Clients"));
  PermClient = p;
  PermGroup.SetName("SubProtocol");
  PermGroup.add(std::const_pointer_cast<const Permission>(p));
}

// *******************************************************************************************
SubProtocol::~SubProtocol()
{
  Clients.erase(Clients.begin(), Clients.end());
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

const PermissionGroup &SubProtocol::getPermGroup() const
{
  return PermGroup;
}

// *******************************************************************************************
#define MAX_FP(a, b)  b = (a > b)? a : b
// *******************************************************************************************
void SubProtocol::Run(struct timeval timeout)
{
  size_t i;

  for ( i = 0; i < Clients.size(); i ++ ) {
    if ( Clients[i]->isConnected() == false ) {
      Clients.erase(Clients.begin() + i);
      break;
    }
  }

  ClientSocket::Client_Ptr p;
  p = Server->Listen(timeout);
  if ( p != NULL ) {
    Clients.push_back(p);
    ResetPacketTime();
  }

  string l;
  for ( i = 0; i < Clients.size(); i ++ ) {
    while ( Clients[i]->Read(l) == true ) {
      ProcessLine(l);
    }
  }

  if ( Clients.size() != 0 ) {
    SendUpdatedData();
    PermClient->Set(true);
  } else {
    PermClient->Set(false);
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
    if ( Modules[j].PTime != Modules[j].module->GetPacketTime() ) {
      Modules[j].PTime = Modules[j].module->GetPacketTime();

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
  for ( i = 0; i < Clients.size(); i ++ ) {
    Clients[i]->Send( msg);
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

  msg += "\"RecordType\":\"Clients\", ";
  msg += "\"Values\":[ ";
  for ( size_t i = 0; i < Clients.size(); i ++ ) {
    msg += "{ \"ip\":\"";
    msg += string(Clients[i]->GetName());
    msg += "\"} ";

    if (( Clients.size() > 1 ) && ( i <  (Clients.size()-1))) {
      msg += ", ";
    }
  }
  msg += "]";
  return msg;
}

// *******************************************************************************************
void SubProtocol::ProcessLine(string line)
{
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
int SubProtocol::GetNumClients(void) const
{
  return Clients.size();
}

// *******************************************************************************************
// *******************************************************************************************
