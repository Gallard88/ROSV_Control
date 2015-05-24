/*
 Power Manager ( http://www.github.com/Gallard88/ROSV_Control )
 Copyright (c) 2013 Thomas BURNS

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

//  *******************************************************************************************
#include <math.h>
#include "PowerManager.h"
#include "EventMessages.h"
#include "MsgQueue.h"

using namespace std;


const char *VoltNames[NUM_VOLTAGE_CH] = {
  "Pwm_Volt",
  "PMon_Pri",
  "PMon_Sec"
};

//  *******************************************************************************************
PowerManager::PowerManager(const char * filename, PWM_Con_t p):
  Pwm(p)
{
  PMon = PMon_Connect();
  TempGroup.SetName("Temp");
  VoltGroup.SetName("Voltage");
  EventMsg *Msg = EventMsg::Init();
  MQue = new MsgQueue("Power", false);

  if ( PMon == NULL ) {
    Msg->Log(EventMsg::ERROR, "PowerManager: failed to open PowerMonitor");
    exit(-1);
  }

  for ( int i = 0; i < NUM_VOLTAGE_CH; i ++ ) {
    Volts[i].SetName("Volt", VoltNames[i]);

    std::shared_ptr<Alarm> a(new Alarm(VoltNames[i], false, true));
    VoltAlarms[i] = a;
    VoltGroup.AddAlarm(std::const_pointer_cast<const Alarm>(a));
  }


  Temp.SetName("Temp", "Pwm_Temp");
  std::shared_ptr<Alarm> t(new Alarm("Pwm_Temp", false, true));
  TempAlarms = t;
  TempGroup.AddAlarm(std::const_pointer_cast<const Alarm>(TempAlarms));
}

PowerManager::~PowerManager()
{
}

MsgQueue *PowerManager::GetQueue(void)
{
  return MQue;
}

//  *******************************************************************************************
const AlarmGroup & PowerManager::getVoltAlarmGroup(void)
{
  return VoltGroup;
}

const AlarmGroup & PowerManager::getTempAlarmGroup(void)
{
  return TempGroup;
}

//  *******************************************************************************************
static Alarm::Severity_t CheckTemp(float temp)
{
  if ( temp < -10.0 || temp > 50.0 )
    return Alarm::ERROR;
  if ( temp < 0.0 || temp > 40.0 )
    return Alarm::WARNING;
  return Alarm::CLEAR;
}

static Alarm::Severity_t CheckVoltage(float v)
{
  if ( v < 10.0 || v > 17.0 )
    return Alarm::ERROR;
  if ( v < 11.0 )
    return Alarm::WARNING;
  return Alarm::CLEAR;
}

void PowerManager::Run_Task(void)
{
  float v[NUM_VOLTAGE_CH], temp;
  bool send_data = false;
  bool send_temp = false;
  bool send_volt = false;
  Alarm::Severity_t alm;

  if ( MQue->IsBroadcast() ) {
    send_data = true;
    send_temp = true;
    send_volt = true;
  }

  PMon_GetVoltages(PMon, &v[1], &v[2]);
  PWM_GetMeasurements(Pwm, &v[0], &temp, NULL);

  for ( int i = 0; i < NUM_VOLTAGE_CH; i ++ ) {
    if ( fabsf(Volts[i].Get() - v[i]) > 0.1 ) {
      Volts[i].Set(v[i]);
      send_data = true;
    }
    alm = CheckVoltage(v[i]);
    if ( alm != LastAlm[i] ) {
      LastAlm[i] = alm;
      send_volt = true;
    }
    VoltAlarms[i]->SetState(alm);
  }

  alm = CheckTemp(temp);
  if ( alm != LastTemp ) {
    LastTemp = alm;
    send_temp = true;
  }
  TempAlarms->SetState(alm);
  Temp.Set(temp);
  if ( send_data == true ) {
    SendData();
  }
  if ( send_volt == true ) {
    MQue->Send("VoltAlarm", VoltGroup.GetJSON());
  }
  if ( send_temp == true ) {
    MQue->Send("TempAlarm", TempGroup.GetJSON());
  }
}

//  *******************************************************************************************
void PowerManager::SendData(void)
{
  string msg = "\"Volt\":[";
  msg += Volts[0].GetJSON() + ", ";
  msg += Volts[1].GetJSON() + ", ";
  msg += Volts[2].GetJSON();
  msg += "], ";

  msg += "\"Temp\":[" + Temp.GetJSON() + "]";
  MQue->Send("PowerData", msg);
}

//*******************************************************************************************
//*******************************************************************************************

