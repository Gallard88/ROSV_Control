/*
 ControlProtocol ( http://www.github.com/Gallard88/ROSV_Control )
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

/* ======================== */
#ifndef __CONTROL_PROTOCOL__
#define __CONTROL_PROTOCOL__

/* ======================== */
#include <string>

#include "DataSource.h"

/* ======================== */

struct Command {
  const char *cmd;
  bool read;
  bool write;
  int func_number;
};

class ControlProtocol {

public:
  ControlProtocol();
  ~ControlProtocol();

  void AddControlSource(DataSource *src);
  bool IsControlSourceConnected(void);
  int GetControlFileDescriptor(void);
  void GetControlData(void);

  void AddObserverSource(DataSource *src);
  int GetNumberOfObservers(void);
  int GetObserverFileDescriptor(void);
  void GetObserverData(void);

  const struct Command *GetCmds(const struct Command *list, string *arg, int *fp);

  void Write(int fd, string msg);
  void Write(int fd, const char *msg);

private:
  DataSource *Controller;
  DataSource *Observer;
};

/* ======================== */
/* ======================== */
#endif
