#ifndef __ALARM_MANAGER__
#define __ALARM_MANAGER__

#include "Alarm.h"

class MsgQueue;

class AlarmManager:  public AlarmGroup
{
public:
  AlarmManager();
  virtual ~AlarmManager();

  void Check(void);

  // Cmd Module Functions.
  MsgQueue *GetQueue(void);

private:
  void SendData(void);
  MsgQueue *MQue;
  Alarm::Severity_t LastState;
};

#endif
