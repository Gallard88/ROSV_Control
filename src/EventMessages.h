#ifndef __EVENT_MESSAGES__
#define __EVENT_MESSAGES__

#include <string>
#include <vector>

#include "CmdModule.h"

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
  void Update(const char *packet, JSON_Object *msg);
  const std::string GetData(void);

private:
  EventMsg();
  ~EventMsg();

  bool PrintSyslog;
  bool PrintTerminal;
  std::string Filename;
  size_t Num_Msgs;
  std::vector<std::string> Messages;

};


#endif
