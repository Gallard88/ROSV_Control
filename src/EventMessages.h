#ifndef __EVENT_MESSAGES__
#define __EVENT_MESSAGES__

#include <string>
#include <vector>

class MsgQueue;

class EventMsg
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

private:
  EventMsg();
  ~EventMsg();

  void SendData(void);
  MsgQueue *MQue;

  bool PrintSyslog;
  bool PrintTerminal;
  std::string Filename;
  size_t Num_Msgs;
  std::vector<std::string> Messages;

};


#endif
