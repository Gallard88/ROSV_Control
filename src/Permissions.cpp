
#include "Permissions.h"
#include "EventMessages.h"

using namespace std;

static EventMsg *Msg;

Permission::Permission(std::string name ):
  Name(name), Value(false)
{
  if ( Msg == NULL ) {
    Msg = EventMsg::Init();
  }
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
      Msg->Log(EventMsg::NOTICE, "Permission %s => enabled", Name.c_str());
    }
  } else {
    if ( Value == true ) {
      Value = false;
      Msg->Log(EventMsg::NOTICE, "Permission %s => disabled", Name.c_str());
    }
  }
}

PermissionGroup::PermissionGroup():
  Name("Not Named")
{
  if ( Msg == NULL ) {
    Msg = EventMsg::Init();
  }
}

PermissionGroup::~PermissionGroup()
{
  PermList.clear();
}

void PermissionGroup::add(std::shared_ptr<const Permission > p)
{
  PermList.push_back(p);
  Msg->Log(EventMsg::NOTICE,"Permission %s added to Group %s ", p->GetName().c_str(), Name.c_str());

}

void PermissionGroup::add(const PermissionGroup & p)
{
  this->PermList.insert(this->PermList.end(), p.PermList.begin(), p.PermList.end());
  Msg->Log(EventMsg::NOTICE, "PermissionGroup %s copied into %s ", p.Name.c_str(), this->Name.c_str());
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
    Msg->Log(EventMsg::NOTICE, "PermissionGroup %s => %s", Name.c_str(), (global==true)? "true": "false");
    GroupState = global;
  }
  return global;
}

std::string PermissionGroup::GetJSON(void)
{
  std::string msg = "\"" + Name + "\":[ ";
  for ( size_t i = 0; i < PermList.size(); i ++ ) {
    msg += "{ \"Name\":\"" + PermList[i]->GetName() + "\", ";
    msg += "\"Enabled\":\"";
    msg += PermList[i]->isEnabled() ? string("True"):string("False");
    msg += "\"}";
    if (( PermList.size() > 1 ) && ( i < (PermList.size() -1) )) {
      msg += ", ";
    }
    msg += "] ";
  }
  return msg;
}

