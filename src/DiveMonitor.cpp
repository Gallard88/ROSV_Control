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
DiveMonitor::DiveMonitor(const JSON_Object *settings, SubControl *sub)
{
  Sub = sub;

  PositionLog = new DataLogger("./", "Position.csv");
  Rec_BearingTitle(PositionLog);


  VelocityLog = new DataLogger("./", "Velocity.csv");
  Rec_BearingTitle(VelocityLog);

  PowerLog = new DataLogger("./", "Power.csv");
  PowerLog->Add_Title("Volt");
  PowerLog->Add_Title("Current");
  PowerLog->Add_Title("Temp");
  PowerLog->RecordLine();
}

//*******************************************************************************************
void DiveMonitor::Rec_BearingTitle(DataLogger *log)
{
  log->Add_Title("Real X");
  log->Add_Title("Real Y");
  log->Add_Title("Real Z");
  log->Add_Title("Real Roll");
  log->Add_Title("Real Pitch");
  log->Add_Title("Real Yaw");
  log->Add_Title("Target X");
  log->Add_Title("Target Y");
  log->Add_Title("Target Z");
  log->Add_Title("Target Roll");
  log->Add_Title("Target Pitch");
  log->Add_Title("Target Yaw");
  log->RecordLine();
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
  Rec_Bearing(PositionLog, INS_GetPosition(), Sub->Position);
  Rec_Bearing(VelocityLog, INS_GetVelocity(), Sub->Velocity);
  Rec_Power();
}

//*******************************************************************************************
void DiveMonitor::Rec_Power(void)
{
  PowerLog->Add_Var(PWM_GetVoltage(Pwm));
  PowerLog->Add_Var(PWM_GetCurrent(Pwm));
  PowerLog->Add_Var(PWM_GetTemp(Pwm));
  PowerLog->RecordLine();
}

//*******************************************************************************************
void DiveMonitor::Rec_Bearing(DataLogger *log, INS_Bearings data, INS_Bearings target)
{
  log->Add_Var(data.x);
  log->Add_Var(data.y);
  log->Add_Var(data.z);
  log->Add_Var(data.roll);
  log->Add_Var(data.pitch);
  log->Add_Var(data.yaw);
  log->Add_Var(target.x);
  log->Add_Var(target.y);
  log->Add_Var(target.z);
  log->Add_Var(target.roll);
  log->Add_Var(target.pitch);
  log->Add_Var(target.yaw);
  log->RecordLine();
}

//*******************************************************************************************
//*******************************************************************************************

