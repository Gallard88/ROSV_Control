/*
 Power Monitor ( http://www.github.com/Gallard88/ROSV_Control )
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

//*******************************************************************************************
using namespace std;

#include <PWM_Controller.h>

#include "time.h"

#include "PowerMonitor.h"

const DataLoggerOpt DL_Opt =
{
  true,	//	filename_date
  false,	//	filename_time
  true,	//	first_col_date
  true	//	first_col_time
};

//*******************************************************************************************
PowerMonitor::PowerMonitor(string path)
{
  if ( !path.size() )
    path += "./";
  Log = new DataLogger(path, "ROSV_Power.csv", &DL_Opt);
  Log->Add_Title("Temp");
  Log->Add_Title("Voltage");
  Log->Add_Title("Current");
  Log->RecordLine();
}

//*******************************************************************************************
void PowerMonitor::Run(void)
{
  static time_t update;
  time_t current;

  time(&current);
  if ( update == current )
    return;
  update = current;

  Log->Add_Var(PWM_GetTemp());
  Log->Add_Var(PWM_GetVoltage());
  Log->Add_Var(PWM_GetCurrent());
  Log->RecordLine();
}

//*******************************************************************************************
//*******************************************************************************************

