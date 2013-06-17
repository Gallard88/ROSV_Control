/*
 DiveMonitor ( http://www.github.com/Gallard88/ROSV_Control )
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

#include "DiveMonitor.h"

//*******************************************************************************************
DiveMonitor::DiveMonitor(const JSON_Object *settings)
{
  PositionLog = new DataLogger("./", "Position.csv");

  PositionLog->Add_Title("X");
  PositionLog->Add_Title("Y");
  PositionLog->Add_Title("Z");
  PositionLog->Add_Title("Roll");
  PositionLog->Add_Title("Pitch");
  PositionLog->Add_Title("Yaw");
  PositionLog->RecordLine();

  VelocityLog = new DataLogger("./", "Velocity.csv");

  VelocityLog->Add_Title("X");
  VelocityLog->Add_Title("Y");
  VelocityLog->Add_Title("Z");
  VelocityLog->Add_Title("Roll");
  VelocityLog->Add_Title("Pitch");
  VelocityLog->Add_Title("Yaw");
  VelocityLog->RecordLine();

  PowerLog = new DataLogger("./", "Power.csv");

  PowerLog->Add_Title("Volt");
  PowerLog->Add_Title("Current");
  PowerLog->Add_Title("Temp");
  PowerLog->RecordLine();
}

//*******************************************************************************************
void DiveMonitor::Run(void)
{
  time_t current = time(NULL);

  if ( current == LastUpdate )
    return;
  // here once a second
  LastUpdate = current;

  // record position into
  Rec_Bearing(PositionLog, INS_GetPosition());
  Rec_Bearing(VelocityLog, INS_GetVelocity());
  Rec_Power();
}

//*******************************************************************************************
void DiveMonitor::Rec_Power(void)
{
  PowerLog->Add_Var(PWM_GetVoltage());
  PowerLog->Add_Var(PWM_GetCurrent());
  PowerLog->Add_Var(PWM_GetTemp());
  PowerLog->RecordLine();
}

//*******************************************************************************************
void DiveMonitor::Rec_Bearing(DataLogger *log, INS_Bearings data)
{
  log->Add_Var(data.x);
  log->Add_Var(data.y);
  log->Add_Var(data.z);
  log->Add_Var(data.roll);
  log->Add_Var(data.pitch);
  log->Add_Var(data.yaw);
  log->RecordLine();
}

//*******************************************************************************************
//*******************************************************************************************

