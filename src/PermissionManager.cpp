#include "PermissionManager.h"

using namespace std;

PermGroupManager::PermGroupManager():
  LastState(false)
{
  SetName("PermissionManager");
}

PermGroupManager::~PermGroupManager()
{
}

void PermGroupManager::Check(void)
{
  bool s = isGroupEnabled();
  if ( s != LastState ) {
    FlagReady();
    LastState = s;
  }
}


void PermGroupManager::Update(const char *packet, JSON_Object *msg)
{
  // nothing to do here.
  return;
}

const std::string PermGroupManager::GetData(void)
{
  string msg;
  char vec[256];

  msg = "\"RecordType\": \"ReportPermission\", ";
  msg += "\"Permissions\":[";

  for ( size_t i = 0; i < PermList.size(); i ++ ) {
    sprintf(vec, "{ \"Permission\":\"%s\", \"State\": \"%s\" }", PermList[i]->GetName().c_str(), PermList[i]->isEnabled()?"En":"Dis");
    msg += string(vec);
    if (( PermList.size() > 0 ) && i < (PermList.size()-1))
      msg += ", ";
  }
  msg += " ]";
  return msg;
}

