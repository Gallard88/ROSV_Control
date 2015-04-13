#ifndef __ALARM_MANAGER__
#define __ALARM_MANAGER__

#include "Alarm.h"
#include "CmdModule.h"

class AlarmManager: public CmdModule, public AlarmGroup
{
public:
  AlarmManager();
  virtual ~AlarmManager();

  // Cmd Module Functions.
  void Update(const char *packet, JSON_Object *msg);
  const std::string GetData(void);

private:

};

#endif
