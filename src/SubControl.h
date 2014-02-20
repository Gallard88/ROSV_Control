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

//*******************************************************************************************
#ifndef __SUB_CONTROL__
#define __SUB_CONTROL__

//*******************************************************************************************
#include <string>
#include <PWM_Controller.h>
#include <INS_Data.h>
#include <vector>

#include "CmdModule.h"
#include "parson.h"

//*******************************************************************************************
enum SubMode {
  Idle = 0,
  Vel,
  Pos
};

//*******************************************************************************************
struct Motor {
  string Name;
  int ch;
  int mult[INS_AXES_SIZE];
  float power;
};

//*******************************************************************************************
class SubControl: CmdModule {
public:
  SubControl(const char *filename);
  void Run(void);
  int SetTargetPos(INS_Bearings pos);
  int SetTargetVel(INS_Bearings vel);

  PWM_Con_t Pwm;
  INS_Bearings Position;
  INS_Bearings Velocity;

  struct Motor ParseJson(const JSON_Object *setting);

  const string GetConfigData(void);
  void Update(const string & msg);
  const string GetData(void);

private:
  vector<struct Motor> MotorList;

  enum SubMode Mode;
};

//*******************************************************************************************
#endif