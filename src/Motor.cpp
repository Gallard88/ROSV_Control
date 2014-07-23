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
Motor::Motor(const JSON_Object *setting, PWM_Con_t p)
{
  printf("new Motor()\n");
  this->update = 0;
  Pwm = p;
  Name = string(json_object_get_string(setting, "Name"));
  Chanel = (int) json_object_get_number(setting, "ch");

  JSON_Array *mult_array = json_object_get_array( setting, "mul");
  if ( mult_array != NULL ) {
    for ( size_t i = 0; i < VECTOR_SIZE; i ++ ) {
      mult[i] = json_array_get_number(mult_array, i);
    }
  }
  Target = 0.0;
  Power = 0.0;
  Log = Logger::Init();
}

//  *******************************************************************************************
void Motor::Run(float *power)
{
  float output = 0.0;

  for ( int j = 0; j < VECTOR_SIZE; j ++ ) {
    output = output + ((float)mult[j] * power[j]);
  }
  this->Target = output;

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
#define RAMP_LIMIT 0.05

  if ( Power < Target ) {
    if  (( Target - Power ) > RAMP_LIMIT )
      Power += RAMP_LIMIT;
    else
      Power = Target;

  } else {
    Power = Target;
  }
  PWM_SetPWM(Pwm, Chanel, Power );
  time_t current;
  current = time(NULL);
  if ((current - update) > 1) {
    update = current;
    Log->RecordValue("Motor", Name.c_str(), Power);
  }
}

//*******************************************************************************************
float Motor::GetPower(void)
{
  return this->Power;
}

//*******************************************************************************************
string Motor::GetName(void)
{
  return this->Name;
}

