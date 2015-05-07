#ifndef __MSG_QUEUE__
#define __MSG_QUEUE__

#include <string>
#include <vector>

class MsgQueue
{
public:
  MsgQueue(std::string module, bool update);

  void SetBroadcast(void);
  void Store(const std::string & msg);
  bool MsgWaiting(void);
  std::string GetMsg(void);

  //
  bool ReceiveReady(void);
  bool Receive(std::string * type, std::string * data);
  void Send(const std::string & record, const std::string & data);
  bool IsBroadcast(void);

private:
  std::string Module;
  bool Update;
  bool Broadcast;
  std::vector<std::string> RxMsgs;
  std::vector<std::string> TxMsgs;

};

#endif

