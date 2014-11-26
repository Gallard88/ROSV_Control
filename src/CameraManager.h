/*
 CameraManager ( http://www.github.com/Gallard88/ROSV_Control )
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
#ifndef __CAMERA_MANAGER__
#define __CAMERA_MANAGER__
//*******************************************************************************************
using namespace std;

#include "RTT_Interface.h"
#include "CmdModule.h"
#include "parson.h"

//*******************************************************************************************
class CameraManager: CmdModule, RTT_Interface {
public:
  CameraManager(const char *filename);
  ~CameraManager();

  void Start(const char *ip);
  void Stop(void);
  long DiveTime(void);

  void Update(const char *packet, JSON_Object *msg);
  const string GetData(void);

  void Run_Task(void);

private:
  char *StartSc;
  char *StopSc;
  time_t StartTime;
};

//*******************************************************************************************
#endif
