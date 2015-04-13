#include "AlarmManager.h"

using namespace std;

AlarmManager::AlarmManager()
{
  SetName("AlarmManager");
}

AlarmManager::~AlarmManager()
{
}

void AlarmManager::Check(void)
{
  PacketTime = time(NULL);
}

void AlarmManager::Update(const char *packet, JSON_Object *msg)
{
  // nothing to do here.
  return;
}

const std::string AlarmManager::GetData(void)
{
  string msg;
  char vec[256];

  msg = "\"RecordType\": \"ReportAlarms\", ";
  msg += "\"Alarms\":[";

  for ( size_t i = 0; i < AlarmList.size(); i ++ ) {
    sprintf(vec, "{ \"Alm\":\"%s\", \"State\": \"%s\" }", AlarmList[i]->GetName().c_str(), AlarmList[i]->GetState_Text().c_str());
    msg += string(vec);
    if (( AlarmList.size() > 0 ) && i < (AlarmList.size()-1))
      msg += ", ";
  }
  msg += " ]";
  return msg;
}

