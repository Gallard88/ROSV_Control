#ifndef __PERMISSION_MANAGER__
#define __PERMISSION_MANAGER__

#include "Permissions.h"
#include "CmdModule.h"

class PermGroupManager: public CmdModule, public PermissionGroup
{
public:
  PermGroupManager();
  virtual ~PermGroupManager();

  // Cmd Module Functions.
  void Update(const char *packet, JSON_Object *msg);
  const std::string GetData(void);

private:

};

#endif
