
#include <syslog.h>

#include "Permissions.h"

Permission::Permission(std::string name ):
  Name(name), Value(false)
{
}

Permission::~Permission()
{
  Name.clear();
}

bool Permission::isEnabled(void) const
{
  return Value;
}

std::string Permission::GetName(void) const
{
  return Name;
}

void Permission::Set(bool en)
{
  if ( en == true ) {
    if ( Value == false ) {
      Value = true;
      syslog(LOG_NOTICE, "Permission %s => enabled", Name.c_str());
    }
  } else {
    if ( Value == true ) {
      Value = false;
      syslog(LOG_NOTICE, "Permission %s => disabled", Name.c_str());
    }
  }
}

PermissionGroup::PermissionGroup():
  Name("Not Named")
{
}

PermissionGroup::~PermissionGroup()
{
  PermList.clear();
}

void PermissionGroup::add(std::shared_ptr<const Permission > p)
{
  PermList.push_back(p);
  syslog(LOG_NOTICE, "Permission %s added to Group %s ", p->GetName().c_str(), Name.c_str());

}

void PermissionGroup::add(const PermissionGroup & p)
{
  this->PermList.insert(this->PermList.end(), p.PermList.begin(), p.PermList.end());
  syslog(LOG_NOTICE, "PermissionGroup %s copied into %s ", p.Name.c_str(), this->Name.c_str());
}

void PermissionGroup::SetName(std::string name)
{
  Name = name;
}

bool PermissionGroup::isGroupEnabled()
{
  bool global = true;

  for ( size_t i = 0; i < PermList.size(); i ++ ) {
    if ( PermList[i]->isEnabled() == false ) {
      global = false;
    }
  }
  if ( GroupState != global ) {
    syslog(LOG_NOTICE, "Alarm Group %s => %s", Name.c_str(), (global==true)? "true": "false");
    GroupState = global;
  }
  return global;
}


