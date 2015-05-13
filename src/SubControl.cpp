/*
 SubControl ( http://www.github.com/Gallard88/ROSV_Control )
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
// *******************************************************************************************
#include <parson.h>

#include "SubControl.h"
#include "EventMessages.h"
#include "MsgQueue.h"

using namespace std;

SubControl::SubControl(const char *filename, PWM_Con_t p):
  Pwm(p)
{
  Motor::SetRamp(1.0);
  Alarms.SetName("SubControl");
  Perm.SetName("SubControl");
  MQue = new MsgQueue("Motor", false);

  EventMsg *Msg = EventMsg::Init();

  JSON_Value *val = json_parse_file(filename);
  int rv = json_value_get_type(val);

  if ( rv != JSONObject ) {
    Msg->Log(EventMsg::ERROR, "SubControl: JSON Parse file failed\n");
    json_value_free (val);
    return;
  }

  JSON_Object *settings = json_value_get_object(val);
  if ( settings == NULL ) {
    Msg->Log(EventMsg::ERROR,"SubControl: Settings == NULL\n");
    json_value_free (val);
    return;
  }

  // load motor data
  JSON_Array *array = json_object_get_array( settings, "Motors");
  if ( array == NULL ) {
    Msg->Log(EventMsg::ERROR, "Failed to find \"Motor\" array in settings");
    return ;
  }

  for ( size_t i = 0; i < json_array_get_count(array); i ++ ) {

    JSON_Object *j_motor = json_array_get_object (array, i);
    if ( j_motor != NULL ) {
      Motor motor(j_motor);
      MotorList.push_back(motor);
    }
  }
  json_value_free (val);
}

MsgQueue *SubControl::GetQueue(void)
{
  return MQue;
}

void SubControl::Add(const AlarmGroup & group)
{
  Alarms.add(group);
}

void SubControl::Add(const PermissionGroup & group)
{
  Perm.add(group);
}

void SubControl::SendData(void)
{
  string msg;
  msg += "\"Channels\":[ ";
  for ( size_t i = 0; i < MotorList.size(); i ++ ) {

    msg += MotorList[i].GetJSON();

    if (( MotorList.size() > 1 ) && ( i < (MotorList.size()-1))) {
      msg += ", ";
    }
  }
  msg += " ] ";
  MQue->Send("MotorData", msg);
}

const float MOT_SCALE = 100.0;

void SubControl::Update(const float * update)
{
  float power[vecSize];
  size_t i;

  bool send_alm = false;
  bool send_perm = false;

  if ( MQue->IsBroadcast() ) {
    send_alm = true;
    send_perm = true;
  }


  if (( Alarms.GetGroupState() == Alarm::ERROR ) ||
      ( Perm.isGroupEnabled() == false )) {

    for ( i = 0; i < vecSize; i ++ ) {
      power[i] = 0.0;
    }
  } else {
    for ( i = 0; i < vecSize; i ++ ) {
      power[i] = update[i] / MOT_SCALE;
    }
  }

  struct PWM_Update new_values[16];

  for ( i = 0; i < MotorList.size(); i ++ ) {
    MotorList[i].Run(power);
    new_values[i].ch = MotorList[i].GetChanel();
    new_values[i].duty = MotorList[i].GetPower();
  }
  PWM_SetMultiplePWM(Pwm, new_values, MotorList.size());


  if ( send_alm == true ) {
    MQue->Send("Alarm", Alarms.GetJSON());
  }
  if ( send_perm == true ) {
    MQue->Send("Permissions", Perm.GetJSON());
  }
  SendData();
}
