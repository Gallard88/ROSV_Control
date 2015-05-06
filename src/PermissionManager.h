#ifndef __PERMISSION_MANAGER__
#define __PERMISSION_MANAGER__

#include "Permissions.h"
#include "CmdModule.h"

class MsgQueue;

class PermGroupManager: public CmdModule, public PermissionGroup
{
public:
  PermGroupManager();
  virtual ~PermGroupManager();

  void Check(void);

  // Cmd Module Functions.
  MsgQueue *GetQueue(void);
  const std::string GetData(void);

private:
  MsgQueue *MQue;
  bool LastState;
};

#endif
