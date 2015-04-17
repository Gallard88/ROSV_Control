#ifndef __CLIENT_SOCKET__
#define __CLIENT_SOCKET__

#include <string>
#include <memory>

class EventMsg;

class ClientSocket
{
public:
  ClientSocket(std::string name, int fp);
  virtual ~ClientSocket();

  typedef std::shared_ptr<ClientSocket> Client_Ptr;

  std::string GetName() const;
  bool isConnected() const;
  int getFp() const;

  void Send(const std::string & msg);
  bool Read(std::string & line);

  void ReadData(void);

private:

  void CloseSocket(void);

  int File;
  std::string Name;
  std::string Buffer;
  EventMsg *Msg;
};

#endif
