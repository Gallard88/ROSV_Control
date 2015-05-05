
#include "ClientSocket.h"
#include "EventMessages.h"

using namespace std;

ClientSocket::ClientSocket(std::string name, int fp):
  File(fp), Name(name)
{
  Buffer.reserve(1024);
  Msg = EventMsg::Init();
  Msg->Log(EventMsg::NOTICE, "New ClientSocket %s", Name.c_str());
}

ClientSocket::~ClientSocket()
{
  Msg->Log(EventMsg::NOTICE, "Delete ClientSocket %s", Name.c_str());
  CloseSocket();
}

void ClientSocket::CloseSocket(void)
{
  if ( File >= 0 ) {
    close(File);
    File = -1;
  }
}

std::string ClientSocket::GetName() const
{
  return Name;
}

bool ClientSocket::isConnected() const
{
  return ( File >= 0 )? true: false;
}

int ClientSocket::getFp() const
{
  return File;
}

void ClientSocket::Send(const std::string & msg)
{
  if ( isConnected() == false ) {
    return;
  }
  int n = write(File, msg.c_str(), msg.size());
  if ( n <= 0 ) {
    CloseSocket();
  }
}

bool ClientSocket::Read(std::string & line)
{
  if ( isConnected() == true ) {
    size_t found;
    while (( found = Buffer.find_first_of("\r\n")) != string::npos ) {
      line = Buffer.substr(0, found);
      Buffer.erase(0, found+1);

      if ( line.length()) {
        return true;
      }
    }
  }
  return false;
}

void ClientSocket::ReadData(void)
{
  char data[8192];
  int n = read(File, data, sizeof(data)-1);
  if ( n <= 0 ) {
    CloseSocket();
  } else {
    data[n] = 0;
    Buffer += string(data);
  }
}

