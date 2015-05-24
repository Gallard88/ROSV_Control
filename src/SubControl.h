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

//  *******************************************************************************************
#ifndef __SUB_CONTROL__
#define __SUB_CONTROL__

//  *******************************************************************************************
#include <string>
#include <vector>
#include <PWM_Controller.h>

#include "Motor.h"
#include "Alarm.h"
#include "Permissions.h"

class MsgQueue;

class SubControl {
public:
  SubControl(const char *filename, PWM_Con_t p);

  typedef enum {
    vecX = 0,
    vecY,
    vecZ,
    vecROLL,
    vecPITCH,
    vecYAW,
    vecSize
  } Control;

  void Add(const AlarmGroup & group);
  void Add(const PermissionGroup & group);

  void Update(const float * update);
  MsgQueue *GetQueue(void);

private:
  void SendData(void);

  Alarm::Severity_t LastAlm;
  bool LastPermission;

  MsgQueue *MQue;
  std::vector<Motor> MotorList;
  AlarmGroup      Alarms;
  PermissionGroup Perm;
  PWM_Con_t Pwm;
};

//*******************************************************************************************
#endif
