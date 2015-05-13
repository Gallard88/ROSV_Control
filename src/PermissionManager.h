#ifndef __PERMISSION_MANAGER__
#define __PERMISSION_MANAGER__

#include "Permissions.h"

class MsgQueue;

class PermGroupManager:  public PermissionGroup
{
public:
  PermGroupManager();
  virtual ~PermGroupManager();

  void Check(void);

  // Cmd Module Functions.
  MsgQueue *GetQueue(void);

private:
  void SendData(void);
  MsgQueue *MQue;
  std::atomic_bool LastState;
};

#endif
