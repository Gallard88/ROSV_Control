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

#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <sys/time.h>

#include "Logger.h"

using namespace std;

//  *******************************************************************************************
Logger::Logger()
{
}

Logger::~Logger()
{
}

//  *******************************************************************************************
void Logger::RecordValue(const char *module, const char *par, float value)
{
  char filename[1024];
  sprintf(filename, "/var/log/ROSV/%s.%s.log", module, par);

  FILE *fp = fopen(filename, "a+");
  if ( fp != NULL ) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm tm = *localtime(&tv.tv_sec);

    fprintf(fp, "%04d-%02d-%02d, %02d:%02d:%02d.%03d, %f\r\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec, (int)(tv.tv_usec / 1000),
            value);
    fclose(fp);
  }
}

//*******************************************************************************************
//*******************************************************************************************

