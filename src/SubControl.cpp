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
using namespace std;

#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "SubControl.h"

// *******************************************************************************************
SubControl::SubControl(const char *filename)
{
  Enable = false;
  memset(&Velocity, 0, sizeof(Velocity));

  JSON_Value *val = json_parse_file(filename);
  int rv = json_value_get_type(val);

  if ( rv != JSONObject ) {
    syslog(LOG_EMERG, "SubControl: JSON Parse file failed\n");
    json_value_free (val);
    return;
  }

  JSON_Object *settings = json_value_get_object(val);
  if ( settings == NULL ) {
    syslog(LOG_EMERG, "SubControl: Settings == NULL\n");
    json_value_free (val);
    return;
  }

  // load motor data
  JSON_Array *array = json_object_get_array( settings, "Motors");
  if ( array == NULL ) {
    syslog(LOG_EMERG,"Failed to find \"Motor\" array in settings");
    printf("Failed to find \"Motor\" array in settings\n");
    return ;
  }

  for ( size_t i = 0; i < json_array_get_count(array); i ++ ) {

    JSON_Object *j_motor = json_array_get_object (array, i);
    if ( j_motor != NULL ) {
      struct Motor motor = ParseJson(j_motor);
      MotorList.push_back(motor);
    }
  }
  json_value_free (val);
}

// *******************************************************************************************
struct Motor SubControl::ParseJson(const JSON_Object *setting) {
  struct Motor mot;

  mot.Name = json_object_get_string(setting, "Name");
  mot.ch = (int) json_object_get_number(setting, "ch");

  JSON_Array *mult_array = json_object_get_array( setting, "mul");
  if ( mult_array != NULL ) {
    for ( size_t i = 0; i < VECTOR_SIZE; i ++ ) {
      mot.mult[i] = json_array_get_number(mult_array, i);
    }
  }
  mot.target = 0.0;
  mot.power = 0.0;
  return mot;
}

// *******************************************************************************************
const string SubControl::GetConfigData(void)
{
  string msg;

  msg = "\"Chanels\":[ ";
  for ( size_t i = 0; i < MotorList.size(); i ++ ) {
    msg += " \"";
    msg += MotorList[i].Name;
    msg += "\"";
    if (( MotorList.size() > 1 ) && ( i < (MotorList.size()-1))) {
      msg += ", ";
    }
  }
  msg += " ] ";
  return msg;
}

// *******************************************************************************************
void SubControl::Update(JSON_Object *msg)
{
  const char *rec_type = json_object_get_string(msg, "RecordType");
  if ( rec_type == NULL ) {
    return ;
  }
  if ( strcmp(rec_type, "Velocity") == 0 ) {
    JSON_Array *vec = json_object_get_array(msg, "Vector");
    if ( vec != NULL ) {
      if ( json_array_get_count(vec) == 4 ) {
        Velocity.x = json_array_get_number(vec, 0);
        Velocity.y = json_array_get_number(vec, 1);
        Velocity.z = json_array_get_number(vec, 2);
        Velocity.yaw = json_array_get_number(vec, 3);
      }
    }
  } else {
    printf("Update: %s\n", rec_type);
  }
}

// *******************************************************************************************
const string SubControl::GetData(void)
{
  char power[10];
  string msg("\"RecordType\": \"Update\", ");
  msg += "\"Chanels\":[ ";
  for ( size_t i = 0; i < MotorList.size(); i ++ ) {
    msg += " {\"Name\": \"";
    msg += MotorList[i].Name;
    msg += "\",\"Max\":100, \"Min\":0, \"Value\": ";
    sprintf(power, "%f", MotorList[i].power * 100 );
    msg += string(power);
    msg += "}";
    if (( MotorList.size() > 1 ) && ( i < (MotorList.size()-1))) {
      msg += ", ";
    }
  }
  msg += " ] ";
  return msg;
}

// *******************************************************************************************
void SubControl::EnableMotor(bool en)
{
  Enable = en;
}


// *******************************************************************************************
/*
 * X
 * Y
 * Z
 * Roll
 * Pitch
 * Yaw
 *
 */
// *******************************************************************************************
const float MOT_SCALE = 100.0;
#define VECTOR_X	0
#define VECTOR_Y	1
#define VECTOR_Z	2
#define VECTOR_ROLL	3
#define VECTOR_PITCH	4
#define VECTOR_YAW	5
#define RAMP_LIMIT	0.05

// *******************************************************************************************
void SubControl::Run(void)
{
  float power[VECTOR_SIZE];

  if ( Enable == false )
    return;

  // run each Axes controller.
  power[VECTOR_X]     = Velocity.x / MOT_SCALE;
  power[VECTOR_Y]     = Velocity.y / MOT_SCALE;
  power[VECTOR_Z]     = Velocity.z / MOT_SCALE;
  power[VECTOR_YAW]   = Velocity.yaw / MOT_SCALE;
  power[VECTOR_ROLL]  = Velocity.roll / MOT_SCALE;
  power[VECTOR_PITCH] = Velocity.pitch / MOT_SCALE;

  for ( size_t i = 0; i < MotorList.size(); i ++ ) {
    float output = 0.0;

    for ( int j = 0; j < VECTOR_SIZE; j ++ ) {
      output = output + ((float)MotorList[i].mult[j] * power[j]);
    }

    MotorList[i].target = output;
  }
  for ( size_t i = 0; i < MotorList.size(); i ++ ) {
    float target = MotorList[i].target;
    float p = MotorList[i].power;

    /* When the motors engage at full power there is often a HUGE surge of current
     * which can cause the main rail to drop a volt or two. This glitch on the supply
     * can often be seen in the more sensitive hardware, such as the light modules
     * flicker a little, and it is sometimes possible for the PWM_Controller and other
     * hardware to disconnect due to the electrical noise.
     *
     * This code is designed to stop the sudden surge on the system by limiting how hard
     * the motors can turn on. Since this only affects the system when increasing the speed
     * of the motors, we only run this code when the motors power is LESS than the target.
     *
     */

    if ( p < target ) {
      if  (( target - p ) > RAMP_LIMIT )
        p += RAMP_LIMIT;
      else
        p = target;

    } else {
      p = target;
    }

    MotorList[i].power = p;
    PWM_SetPWM(Pwm, MotorList[i].ch, p );
  }
}

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
