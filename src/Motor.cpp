/*
 Logger ( http://www.github.com/Gallard88/ROSV_Control )
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
#include "Motor.h"

using namespace std;

float Motor::Ramp = 100.0;
static const float Max = 100.0;
static const float Min = 0.0;

Motor::Motor(const JSON_Object *setting):
  Value(0.0), LastValue(-1.0)
{
  const char *name = json_object_get_string(setting, "Name");
  Name = string(name);

  Chanel = (int) json_object_get_number(setting, "ch");

  JSON_Array *mult_array = json_object_get_array( setting, "mul");
  if ( mult_array != NULL ) {
    for ( size_t i = 0; i < VECTOR_SIZE; i ++ ) {
      mult[i] = json_array_get_number(mult_array, i);
    }
  }
}

Motor::~Motor()
{
}

void Motor::SetRamp(float ramp)
{
  Ramp = ramp;
}

string Motor::GetJSON(void)
{
  char buf[100];

  sprintf(buf, "{ \"Name\": \"%s\", \"Value\": %f }", Name.c_str(), Value );
  return string(buf);
}

float Motor::GetPower(void)
{
  return Value;
}

int Motor::GetChanel(void)
{
  return Chanel;
}

bool Motor::Run(const float *power)
{
  float target = 0.0;

  for ( int j = 0; j < VECTOR_SIZE; j ++ ) {
    target = target + ((float)mult[j] * power[j]);
  }

  if ( target > Max )
    target = Max;
  else if ( target < Min )
    target = Min;

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
  if ( Value < target ) {
    if  (( target - Value ) > Ramp )
      Value += Ramp;
    else
      Value = target;

  } else {
    Value = target;
  }
  if ( LastValue != Value ) {
    LastValue = Value;
    return true;
  }
  return false;
}

