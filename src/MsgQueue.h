#ifndef __MSG_QUEUE__
#define __MSG_QUEUE__

#include <string>
#include <vector>

class MsgQueue
{
public:
  MsgQueue(std::string module, bool update);


  void Store(const std::string & msg);

  //
  bool Receive(std::string * type, std::string * data);

private:
  std::string Module;
  bool Update;
  std::vector<std::string> RxMsgs;

};

#endif

