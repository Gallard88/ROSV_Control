/*
 Logger ( http://www.github.com/Gallard88/ROSV_Control )
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

#include <syslog.h>
#include <cstdlib>  // for exit()
#include <Snotra.h>

#include "Logger.h"

//  *******************************************************************************************
Logger::Logger()
{
  if ( Snotra_Connect() < 0 ) {
    syslog(LOG_ALERT, "Logger() failed\n");
    exit(-1);
  }
}

//  *******************************************************************************************
void Logger::RecordValue(const char *module, const char *par, float value)
{
  Snotra_Send(module, par, value);
}

//*******************************************************************************************
Logger *Logger::Init(void)
{
  static Logger *l;
  if ( l == NULL ) {
    l = new Logger;
  }
  return l;
}

//*******************************************************************************************

