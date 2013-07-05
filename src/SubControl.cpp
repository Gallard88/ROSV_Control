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
//*******************************************************************************************
using namespace std;

#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "SubControl.h"

//*******************************************************************************************
SubControl::SubControl(const JSON_Object *settings)
{
  JSON_Array *array;
  int i;

//	Mode = Idle;
  Mode = Vel;
	memset(&Position, 0, sizeof(INS_Bearings));
	memset(&Velocity, 0, sizeof(INS_Bearings));

  // load motor data
  array = json_object_get_array( settings, "Motor");
  if ( array == NULL ) {
    syslog(LOG_EMERG,"Failed to find \"Motor\" array in settings");
    printf("Failed to find \"Motor\" array in settings\n");
    return ;
  }

  NumMotor = json_array_get_count(array);
  if ( NumMotor == 0 )
    return ;

  MotorList = new struct Motor[NumMotor];
  memset(MotorList, 0, sizeof(struct Motor));

  for ( i = 0; i < NumMotor; i++) {
    struct Motor *mptr = &MotorList[i];

    const char *ptr = json_array_get_string(array, i);
    if ( ptr == NULL )
      continue;
    std::string name(ptr);

    string var_name = name + "." + "ch";
    mptr->ch = (int)json_object_dotget_number(settings, var_name.c_str());

    var_name = name + "." + "mul";
    JSON_Array *scale = json_object_dotget_array(settings, var_name.c_str());
    if ( scale == NULL ) {
      continue;
    }
    for ( int j = 0; j < INS_AXES_SIZE; j ++ ) {
      mptr->mult[j] = json_array_get_number(scale, j);
    }
  }
}

//*******************************************************************************************
/*
 * X
 * Y
 * Z
 * Roll
 * Pitch
 * Yaw
 *
 */
//*******************************************************************************************
void SubControl::Run(void)
{
  float power[INS_AXES_SIZE];
  memset( power, 0, sizeof(float) * INS_AXES_SIZE);

  switch ( Mode ) {

  case Vel:
    // run each Axes controller.
    power[X] = Velocity.x;
    power[Y] = Velocity.y;
    power[Z] = Velocity.z;
    power[ROLL] = Velocity.roll;
    power[PITCH] = Velocity.pitch;
    power[YAW] = Velocity.yaw;
    break;

  case Pos:
    break;

  case Idle:
  default:
    break;
  }
  /* Update Motors */
  for ( int i = 0; i < NumMotor; i ++ ) {
    float output = 0;
    for ( int j = 0; j < INS_AXES_SIZE; j ++ ) {
      output += MotorList[i].mult[j] * power[j];
    }
    PWM_SetPWM(MotorList[i].ch, output);
  }
}

//*******************************************************************************************
int SubControl::SetTargetPos(INS_Bearings pos)
{
  Position = pos;
  return 0;
}

//*******************************************************************************************
int SubControl::SetTargetVel(INS_Bearings vel)
{
  Velocity = vel;
  return 0;
}

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
