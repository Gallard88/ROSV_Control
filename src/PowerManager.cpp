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
#include <syslog.h>
#include <math.h>
#include "PowerManager.h"

using namespace std;

//  *******************************************************************************************
PowerManager::PowerManager(const char * filename, PWM_Con_t p):
  Pwm(p)
{
  PMon = PMon_Connect();

  if ( PMon == NULL ) {
    syslog(LOG_EMERG, "PowerManager: failed to open PowerMonitor");
    exit(-1);
  }

  Volts[0].SetName("Pwm_Volt");
  Volts[1].SetName("PMon_Pri");
  Volts[2].SetName("PMon_Sec");
  Temp.SetName("Pwm_Temp");
}

//  *******************************************************************************************
void PowerManager::Run_Task(void)
{
  PacketTime = time(NULL);
  Volts[0].Set(PWM_GetVoltage(Pwm));

  float v = PMon_GetVoltage(PMon, 0);
  if ( fabsf(Volts[1].Get() - v) > 0.1 ) {
    Volts[1].Set(v);
  }

  v = PMon_GetVoltage(PMon, 1);
  if ( fabsf(Volts[2].Get() - v) > 0.1 ) {
    Volts[2].Set(v);
  }
  Temp.Set(PWM_GetTemp(Pwm));
}

//  *******************************************************************************************
void PowerManager::Update(const char *packet, JSON_Object *msg)
{
}

//  *******************************************************************************************
const string PowerManager::GetData(void)
{
  string jVolt = "\"Volt\":[";
  jVolt += Volts[0].GetJSON() + ", ";
  jVolt += Volts[1].GetJSON() + ", ";
  jVolt += Volts[2].GetJSON();
  jVolt += "]";

  string jTemp = "\"Temp\":[" + Temp.GetJSON() + "]";

  return "\"RecordType\": \"PowerData\", " + jVolt + ", " + jTemp;
}

//*******************************************************************************************
//*******************************************************************************************

