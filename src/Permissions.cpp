
#include <syslog.h>

#include "Permissions.h"

Permission::Permission(std::string name ):
  Name(name), Value(0)
{
}

Permission::~Permission()
{
  Name.clear();
}

bool Permission::isEnabled(void)
{
  return ( Value != 0 )? true: false;
}

void Permission::Inc(void)
{
  if ( Value == 0 ) {
    // now enabled, must log this.
    Set(true);
  } else {
    Value ++;
  }
}

void Permission::Dec(void)
{
  if ( Value == 1 ) {
    Set(false);

  } else if ( Value > 1 ) {
    Value --;
  }
}

void Permission::Set(bool en)
{
  if ( en == true ) {
    if ( Value == 0 ) {
      Value = 1;
      syslog(LOG_NOTICE, "Permission %s => enabled", Name.c_str());
    }
  } else {
    if ( Value != 0 ) {
      Value = 0;
      syslog(LOG_NOTICE, "Permission %s => disabled", Name.c_str());
    }
  }
}

