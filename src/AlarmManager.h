#ifndef __ALARM_MANAGER__
#define __ALARM_MANAGER__

#include "Alarm.h"
#include "CmdModule.h"

class MsgQueue;

class AlarmManager: public CmdModule, public AlarmGroup
{
public:
  AlarmManager();
  virtual ~AlarmManager();

  void Check(void);

  // Cmd Module Functions.
  MsgQueue *GetQueue(void);
  const std::string GetData(void);

private:
  MsgQueue *MQue;
  Alarm::Severity_t LastState;
};

#endif
