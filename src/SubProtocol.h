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
//*******************************************************************************************
#ifndef __SUBPROTOCOL__
#define __SUBPROTOCOL__
//*******************************************************************************************
#include <vector>
#include <memory>

#include "Permissions.h"
#include "TcpServer.h"
#include "ClientSocket.h"

class MsgQueue;

//*******************************************************************************************
class SubProtocol {
public:
  SubProtocol();
  virtual ~SubProtocol();

  void Add(MsgQueue *que);
  void Run(struct timeval timeout);
  int GetNumClients(void) const;
  const PermissionGroup &getPermGroup() const;

protected:

private:
  MsgQueue *MQue;
  TcpServer                   *Server;
  std::shared_ptr<Permission>  PermClient;
  PermissionGroup              PermGroup;
  std::vector<ClientSocket::Client_Ptr> Clients;
  std::vector<MsgQueue *>      ModList;

  void SendServerId(void);
  void SendClientData(void);
  void SendUpdatedData(void);
  void SendMsg(const std::string & msg);
  void ProcessLine(const std::string & line);
};

//*******************************************************************************************
//*******************************************************************************************
#endif
