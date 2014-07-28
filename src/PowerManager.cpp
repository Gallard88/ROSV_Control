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
using namespace std;

#include <syslog.h>

#include <stdio.h>
#include <string.h>
#include <string>

#include "PowerManager.h"
#include "parson.h"

//  *******************************************************************************************
PowerManager::PowerManager(const char * filename)
{
  JSON_Value *val = json_parse_file(filename);
  int rv = json_value_get_type(val);

  SetCallPeriod(1000);

  if ( rv != JSONObject ) {
    syslog(LOG_EMERG, "PowerManager: JSON Parse file failed\n");
    json_value_free (val);
    return;
  }

  JSON_Object *settings = json_value_get_object(val);
  if ( settings == NULL ) {
    syslog(LOG_EMERG, "PowerManager: Settings == NULL\n");
    json_value_free (val);
    return;
  }
  AmpHour         = (float)json_object_get_number (settings, "AmpHour");
  WarningVoltage  = (float)json_object_get_number (settings, "WarningVoltage");
  AlarmVoltage    = (float)json_object_get_number (settings, "AlarmVoltage");
  json_value_free (val);
}

//  *******************************************************************************************
void PowerManager::Run(void)
{
  /* The PWM has a ~7 second watchdog.
  *	If this function is run once per second it should be fast enough
  */
  if ( RunModule() == true ) {

    CurrentVoltage = PWM_GetVoltage(Pwm);
    Log->RecordValue("Power", "Voltage", CurrentVoltage);
  }
}

//  *******************************************************************************************
void PowerManager::Update(JSON_Object *msg)
{
}

//  *******************************************************************************************
const string PowerManager::GetConfigData(void)
{
  return "\"Chanels\":[ \"Voltage\" ] ";
}

//  *******************************************************************************************
const string PowerManager::GetData(void)
{
  char power[10];
  string msg("\"RecordType\": \"Update\", ");

  msg += "\"Chanels\":[ ";
  msg += " {\"Name\": \"Voltage\",\"Max\":24, \"Min\":0, \"Value\": ";
  sprintf(power, "%f", CurrentVoltage );
  msg += string(power);
  msg += "}";
  msg += " ] ";
  return msg;
}

//*******************************************************************************************
//*******************************************************************************************

