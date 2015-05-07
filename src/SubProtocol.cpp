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
#include <sys/time.h>
#include <cmath>
#include <iostream>

#include <parson.h>
#include "SubProtocol.h"
#include "config.h"
#include "MsgQueue.h"

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
  MQue = new MsgQueue("Server", false);
  std::shared_ptr<Permission> p(new Permission("Clients"));
  PermClient = p;
  PermGroup.SetName("SubProtocol");
  PermGroup.add(std::const_pointer_cast<const Permission>(p));
  Add(MQue);
}

// *******************************************************************************************
SubProtocol::~SubProtocol()
{
  Clients.erase(Clients.begin(), Clients.end());
}

// *******************************************************************************************
void SubProtocol::Add(MsgQueue *que)
{
  ModList.push_back(que);
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
    SendServerId(p);
    Clients.push_back(p);
    for ( size_t i = 0; i < ModList.size(); i ++ ) {
      ModList[i]->SetBroadcast();
    }
  }

  string l;
  for ( i = 0; i < Clients.size(); i ++ ) {
    while ( Clients[i]->Read(l) == true ) {
      ProcessLine(l);
    }
  }

  SendUpdatedData();
  PermClient->Set((Clients.size() != 0)? true: false);
}

// *******************************************************************************************
void SubProtocol::SendServerId(ClientSocket::Client_Ptr p)
{
  string msg;
  char name[256];
  gethostname(name, sizeof(name));

  msg += "{ \"Module\":\"Server\", ";
  msg += "\"RecordType\":\"Id\", ";
  msg += "\"Host\":\"" + string(name) + "\", ";
  msg += "\"Type\":\"" + string(PACKAGE_NAME) + "\", ";
  msg += "\"Version\":\"" + string(PACKAGE_VERSION) + "\"";
  msg += "}\r\n";

  p->Send(msg);
}

// *******************************************************************************************
void SubProtocol::SendUpdatedData(void)
{
  string msg;

  for ( size_t i = 0; i < ModList.size(); i ++ ) {
    while ( ModList[i]->MsgWaiting() == true ) {
      msg = ModList[i]->GetMsg();
      for ( size_t i = 0; i < Clients.size(); i ++ ) {
        Clients[i]->Send( msg);
      }
    }
  }
}

//*******************************************************************************************
void SubProtocol::SendMsg(const string & msg)
{
  for ( size_t i = 0; i < Clients.size(); i ++ ) {
    Clients[i]->Send( msg);
  }
}

// *******************************************************************************************
void SubProtocol::SendData(void)
{
  string msg;

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
  MQue->Send("Clients", msg);
}

// *******************************************************************************************
void SubProtocol::ProcessLine(const string & line)
{
  for ( size_t i = 0; i < ModList.size(); i ++ ) {
    ModList[i]->Store(line);
  }
}

// *******************************************************************************************
int SubProtocol::GetNumClients(void) const
{
  return Clients.size();
}

// *******************************************************************************************
// *******************************************************************************************
