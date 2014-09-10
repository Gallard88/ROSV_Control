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
  Val = new Value(name, min, max);
}

//  *******************************************************************************************
string Motor::GetJSON(void)
{
  return Val->GetJSON();
}

//  *******************************************************************************************
void Motor::SetRamp(float ramp)
{
  Val->SetRamp(ramp);
}

//  *******************************************************************************************
void Motor::Run(float *power)
{
  float output = 0.0;

  for ( int j = 0; j < VECTOR_SIZE; j ++ ) {
    output = output + ((float)mult[j] * power[j]);
  }
  PWM_SetPWM(Pwm, Chanel, Val->Run(output));
}

//*******************************************************************************************
float Motor::GetPower(void)
{
  return Val->GetPower();
}

//*******************************************************************************************
string Motor::GetName(void)
{
  return Val->GetName();
}

