#include "AlarmManager.h"
#include "MsgQueue.h"

using namespace std;

AlarmManager::AlarmManager():
  LastState(Alarm::CLEAR)
{
  SetName("AlarmManager");
  MQue = new MsgQueue("AlarmManager", false);
}

AlarmManager::~AlarmManager()
{
  delete MQue;
}

MsgQueue *AlarmManager::GetQueue(void)
{
  return MQue;
}

void AlarmManager::Check(void)
{
  Alarm::Severity_t s = GetGroupState();
  if ( s != LastState ) {
    SendData();
    LastState = s;
  }
}

void AlarmManager::SendData(void)
{
  string msg;
  char vec[256];

  msg = "\"Alarms\":[";

  for ( size_t i = 0; i < AlarmList.size(); i ++ ) {
    sprintf(vec, "{ \"Alm\":\"%s\", \"State\": \"%s\" }", AlarmList[i]->GetName().c_str(), AlarmList[i]->GetState_Text().c_str());
    msg += string(vec);
    if (( AlarmList.size() > 0 ) && i < (AlarmList.size()-1))
      msg += ", ";
  }
  msg += " ]";
  MQue->Send("ReportAlarms", msg);
}

