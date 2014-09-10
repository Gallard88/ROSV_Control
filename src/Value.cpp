/*
 Value ( http://www.github.com/Gallard88/ROSV_Control )
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

#include <string>

#include "Value.h"

//  *******************************************************************************************
Value::Value(const char *name, float min, float max)
{
  this->update = 0;
  Min = min;
  Max = max;
  Ramp = Max;

  Name = string(name);

  Target = 0.0;
  Power = 0.0;
  Log = Logger::Init();
}

//  *******************************************************************************************
string Value::GetJSON(void)
{
  string json;
  char buf[100];

  json = "{ \"Name\": \"" + Name +"\", ";
  sprintf(buf, " \"Min\": %f, ", Min);
  json += string(buf);
  sprintf(buf, " \"Max\": %f, ", Max);
  json += string(buf);
  sprintf(buf, " \"Value\": %f ", Power);
  json += string(buf);
  sprintf(buf, " \"Target\": %f ", Target);
  json += string(buf);
  json += "}";

  return json;
}

//  *******************************************************************************************
void Value::SetRamp(float ramp)
{
  Ramp = ramp;
}

//  *******************************************************************************************
float Value::Run(float power)
{
  if ( power > Max )
    this->Target = Max;
  else if ( power < Min )
    this->Target = Min;
  else
    this->Target = power;

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

  if ( Power < Target ) {
    if  (( Target - Power ) > Ramp )
      Power += Ramp;
    else
      Power = Target;

  } else {
    Power = Target;
  }
  time_t current = time(NULL);
  if ((current - update) > 1) {
    update = current;
    Log->RecordValue("Value", Name.c_str(), Power);
  }
  return Power;
}

// *******************************************************************************************
float Value::GetPower(void)
{
  return this->Power;
}

// *******************************************************************************************
string Value::GetName(void)
{
  return this->Name;
}

// *******************************************************************************************
// *******************************************************************************************
