/*
 Light Manager ( http://www.github.com/Gallard88/ROSV_Control )
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
#ifndef __LIGHT_MANAGER__
#define __LIGHT_MANAGER__
//*******************************************************************************************
#include <string>
#include <vector>
#include <memory>

#include <PWM_Controller.h>

#include "RTT_Interface.h"
#include "CmdModule.h"
#include "Variable.h"
#include "Alarm.h"
#include "Permissions.h"

//*******************************************************************************************
//*******************************************************************************************
typedef struct  {
  Variable Power;
  std::vector<int> Modules;
} LightChanel;

//*******************************************************************************************

class  LightManager: CmdModule, RTT_Interface {
public:
  LightManager(const char * filename);
  ~LightManager();

  void Add(const AlarmGroup & alarm);
  void Add(const PermissionGroup & p);

  void Run_Task(void);

  void Update(const char *packet, JSON_Object *msg);
  const std::string GetData(void);

  PWM_Con_t Pwm;

private:
  std::vector<LightChanel> Chanels;
  PermissionGroup PermGroup;
  AlarmGroup      Alarms;
};

//*******************************************************************************************
//*******************************************************************************************
#endif

