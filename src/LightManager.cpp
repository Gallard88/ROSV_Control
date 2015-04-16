/*
 Light Manager ( http://www.github.com/Gallard88/ROSV_Control )
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

#include <stdio.h>
#include <string.h>
#include <string>

#include "LightManager.h"
#include "parson.h"
#include "EventMessages.h"

using namespace std;

//  *******************************************************************************************
LightManager::LightManager(const char * filename)
{
  Alarms.SetName("Lighting");
  PermGroup.SetName("Lighting");

  JSON_Value *val = json_parse_file(filename);
  int rv = json_value_get_type(val);
  EventMsg *Msg = EventMsg::Init();

  if ( rv != JSONObject ) {
    Msg->Log(EventMsg::ERROR, "Lighting: JSON Parse file failed\n");
    json_value_free (val);
    return;
  }

  JSON_Object *settings = json_value_get_object(val);
  if ( settings == NULL ) {
    Msg->Log(EventMsg::ERROR, "Lighting: Settings == NULL\n");
    json_value_free (val);
    return;
  }
  JSON_Array *ch_array = json_object_get_array( settings, "Modules");
  if ( ch_array == NULL ) {
    Msg->Log(EventMsg::ERROR, "Lighting: Failed to find \"Modules\" array in settings");
    json_value_free (val);
    return ;
  }

  size_t number = json_array_get_count(ch_array);
  if ( number != 0 ) {
    for ( size_t i = 0; i < number; i ++) {
      LightChanel ch;
      ch.Modules.clear();

      JSON_Object *object = json_array_get_object (ch_array, i);
      if ( object == NULL ) {
        return;
      }
      const char *name = json_object_get_string (object, "Name");
      if ( name != NULL ) {
        ch.Power.SetName(name);
      }

      JSON_Array *pwm_array = json_object_get_array( object, "Chanels");
      if ( pwm_array != NULL ) {
        for ( size_t j = 0; j < json_array_get_count(pwm_array); j++ ) {
          int value = (int) json_array_get_number (pwm_array, j);
          ch.Modules.push_back(value);
        }
      }
      Chanels.push_back(ch);
    }
  }
  json_value_free (val);
}

LightManager::~LightManager()
{
  for ( size_t i = 0; i < Chanels.size(); i ++ ) {
    LightChanel ch = Chanels[i];
    for ( size_t j = 0; j < ch.Modules.size(); j ++ ) {
      PWM_SetPWM(Pwm, ch.Modules[j], 0);
    }
  }
}

void LightManager::Add(const AlarmGroup & alarm)
{
  Alarms.add(alarm);
}

void LightManager::Add(const PermissionGroup & p)
{
  PermGroup.add(p);
}

void LightManager::Run_Task(void)
{
  bool en = true;
  int  pwr;

  if (( Alarms.GetGroupState() == Alarm::ERROR ) ||
      ( PermGroup.isGroupEnabled() == false )) {

    en = false;
  }

  for ( size_t i = 0; i < Chanels.size(); i ++ ) {
    LightChanel ch = Chanels[i];
    for ( size_t j = 0; j < ch.Modules.size(); j ++ ) {

//      pwr = ( en == true )? ch.Power.Get(): 0;
      pwr = ( en == true )? 1: 0;
      PWM_SetPWM(Pwm, ch.Modules[j], pwr);
    }
  }
}

void LightManager::Update(const char *packet, JSON_Object *msg)
{
  PacketTime = time(NULL);
}

const string LightManager::GetData(void)
{
  string msg;

  msg = "\"RecordType\":\"LightChanels\", \"Chanels\":[ ";
  for ( size_t i = 0; i < Chanels.size(); i ++ ) {

    msg += Chanels[i].Power.GetJSON();
    if (( Chanels.size() > 1 ) && ( i < (Chanels.size()-1))) {
      msg += ", ";
    }
  }
  msg += " ] ";
  return msg;
}



