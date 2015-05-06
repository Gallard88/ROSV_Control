#ifndef __EVENT_MESSAGES__
#define __EVENT_MESSAGES__

#include <string>
#include <vector>

#include "CmdModule.h"

class MsgQueue;

class EventMsg: CmdModule
{
public:
  static EventMsg * Init();

  void sendToTerminal(bool en);
  void setFilename(const std::string & filename);
  void setLogDepth(int log_depth);

  typedef enum {
    DEBUG,
    INFO,
    NOTICE,
    WARN,
    ERROR
  } EventMsg_t;

  void Log(EventMsg_t type, const char *fmt, ...);

  // Cmd Module Functions.
  MsgQueue *GetQueue(void);
  const std::string GetData(void);

private:
  EventMsg();
  ~EventMsg();

  MsgQueue *MQue;

  bool PrintSyslog;
  bool PrintTerminal;
  std::string Filename;
  size_t Num_Msgs;
  std::vector<std::string> Messages;

};


#endif
