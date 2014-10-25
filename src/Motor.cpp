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
using namespace std;

#include <stdio.h>
#include <string.h>
#include <string>

#include "Motor.h"

//  *******************************************************************************************
Motor::Motor(const JSON_Object *setting, PWM_Con_t p, float min, float max)
{
  Pwm = p;
  const char *name = json_object_get_string(setting, "Name");
  Chanel = (int) json_object_get_number(setting, "ch");

  JSON_Array *mult_array = json_object_get_array( setting, "mul");
  if ( mult_array != NULL ) {
    for ( size_t i = 0; i < VECTOR_SIZE; i ++ ) {
      mult[i] = json_array_get_number(mult_array, i);
    }
  }
  Val.SetName(name);
  Min = min;
  Max = max;
  Ramp = max;
  Target = 0.0;
}

//  *******************************************************************************************
string Motor::GetJSON(void)
{
  return Val.GetJSON();
}

//  *******************************************************************************************
void Motor::SetRamp(float ramp)
{
  Ramp = ramp;
}

//  *******************************************************************************************
void Motor::Run(float *power)
{
  Target = 0.0;

  for ( int j = 0; j < VECTOR_SIZE; j ++ ) {
    Target = Target + ((float)mult[j] * power[j]);
  }

  if ( Target > Max )
    Target = Max;
  else if ( Target < Min )
    Target = Min;

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
  float p = Val.Get();

  if ( p < Target ) {
    if  (( Target - p ) > Ramp )
      p += Ramp;
    else
      p = Target;

  } else {
    p = Target;
  }
  Val.Set(p);
  PWM_SetPWM(Pwm, Chanel, p);
}

//*******************************************************************************************
