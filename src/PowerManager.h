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

//*******************************************************************************************
#ifndef __POWER_MANAGER__
#define __POWER_MANAGER__
//*******************************************************************************************
#include <string>
#include <memory>
#include <PWM_Controller.h>
#include <PowerMonitor.h>

#include "CmdModule.h"
#include "RTT_Interface.h"
#include "Variable.h"
#include "Alarm.h"

//*******************************************************************************************
#define NUM_VOLTAGE_CH  3
//*******************************************************************************************
class MsgQueue;

class  PowerManager: CmdModule, RealTime::Task_Interface {

public:
  PowerManager(const char * filename, PWM_Con_t p);
  virtual ~PowerManager();

  void Run_Task(void);

  MsgQueue *GetQueue(void);
  const std::string GetData(void);

  const AlarmGroup & getVoltAlarmGroup(void);
  const AlarmGroup & getTempAlarmGroup(void);

private:

  AlarmGroup VoltGroup;
  AlarmGroup TempGroup;

  Variable Volts[NUM_VOLTAGE_CH];
  Variable Temp;

  // these need to become a shared pointer...
  std::shared_ptr<Alarm> VoltAlarms[NUM_VOLTAGE_CH];
  std::shared_ptr<Alarm> TempAlarms;

  MsgQueue *MQue;

  PMon_t    PMon;
  PWM_Con_t Pwm;
};

//*******************************************************************************************
//*******************************************************************************************
#endif

