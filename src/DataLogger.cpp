/*
 Data Logger ( http://www.github.com/Gallard88/ROSV_Control )
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
#include <time.h>

using namespace std;

#include "DataLogger.h"

//*******************************************************************************************
DataLogger::DataLogger(const string filename, int opt)
{
  options = opt;
  name = filename;
}

//*******************************************************************************************
DataLogger::~DataLogger()
{
}

//*******************************************************************************************
void DataLogger::ChecktoAddTime(void)
{
	if (( options & DL_ADD_TIME ) && ( line_started == false ))
	{
		time_t current;

		time(&current);
		writer << current;
		writer << ",";
		line_started = true;
	}
}

//*******************************************************************************************
void DataLogger::ChecktoAddTimeTitle(void)
{
	if (( options & DL_ADD_TIME ) && ( line_started == false ))
	{
		writer << "Time,";
		line_started = true;
	}
}

//*******************************************************************************************
void DataLogger::Add_Title(const string field)
{
	if (! writer.is_open())
		writer.open (name.c_str(), ios_base::app);
	ChecktoAddTimeTitle();
	writer << field.c_str();
	writer << ",";
	writer.flush();

	if (!( options & DL_LEAVE_FILE_OPEN ))
		writer.close();
	line_started = true;
}

//*******************************************************************************************
void DataLogger::Add_Var(const string field)
{
	if (! writer.is_open())
		writer.open (name.c_str(), ios_base::app);
	ChecktoAddTime();

	writer << "\"";
	writer << field;
	writer << "\", ";
	writer.flush();
	if (!( options & DL_LEAVE_FILE_OPEN ))
		writer.close();
	line_started = true;
}

//*******************************************************************************************
void DataLogger::Add_Var(const int field)
{
	if (! writer.is_open())
		writer.open (name.c_str(), ios_base::app);
	ChecktoAddTime();

	writer << field;
	writer << ", ";
	writer.flush();
	if (!( options & DL_LEAVE_FILE_OPEN ))
		writer.close();
	line_started = true;
}

//*******************************************************************************************
void DataLogger::Add_Var(const float field)
{
	if (! writer.is_open())
		writer.open (name.c_str(), ios_base::app);
	ChecktoAddTime();

	writer << field;
	writer << ", ";
	writer.flush();
	if (!( options & DL_LEAVE_FILE_OPEN ))
		writer.close();
	line_started = true;
}

//*******************************************************************************************
void DataLogger::RecordLine(void)
{
	if (! writer.is_open())
		writer.open (name.c_str(), ios_base::app);
	ChecktoAddTime();

	writer << "\r\n";
	writer.flush();
	if (!( options & DL_LEAVE_FILE_OPEN ))
		writer.close();
	line_started = false;
}


//*******************************************************************************************
//*******************************************************************************************

