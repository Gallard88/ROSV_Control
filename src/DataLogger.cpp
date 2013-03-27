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

// *******************************************************************************************
#include <time.h>
#include <sstream>

using namespace std;

#include "DataLogger.h"

// *******************************************************************************************
DataLogger::DataLogger(const string path, const string filename, const DataLoggerOpt *options)
{
  ofstream writer;

  time_t current;
  struct tm *loc_time;

  current = time(NULL);
  loc_time = localtime(&current);

  Col_Date = options->first_col_date;
  Col_Time = options->first_col_time;

  Name = path;
  if ( options->filename_date )
  {
    char date[100];

    snprintf(date, sizeof(date),"%04d%02d%02d", loc_time->tm_year + 1900, loc_time->tm_mon+1, loc_time->tm_mday);
    string format(date);

    Name += format;
    Name += "_";
  }
  if ( options->filename_time )
  {
    char time[100];

    snprintf(time, sizeof(time),"%02d%02d%02d", loc_time->tm_hour, loc_time->tm_min, loc_time->tm_sec);
    string format(time);

    Name += format;
    Name += "_";
  }

  Name += filename;
  writer.open(Name.c_str());
  writer.close();
  Line_Started = false;
}

// *******************************************************************************************
DataLogger::~DataLogger()
{
}

// *******************************************************************************************
void DataLogger::ChecktoAddTime(void)
{
  ofstream writer;

  if ( Line_Started == true )
    return ;

  writer.open(Name.c_str(), ios_base::app);

  time_t current;
  struct tm *loc_time;

  current = time(NULL);
  loc_time = localtime(&current);


  if ( Col_Date )
  {
    writer << loc_time->tm_year+1900 <<"/";
    writer << loc_time->tm_mon+1 <<"/";
    writer << loc_time->tm_mday;
    if ( Col_Time == true )
      writer << ",";

  }

  if ( Col_Time )
  {
    writer << loc_time->tm_hour << ":";
    writer << loc_time->tm_min << ":";
    writer << loc_time->tm_sec;
  }
  Line_Started = true;
  writer.close();
}

// *******************************************************************************************
void DataLogger::ChecktoAddTimeTitle(void)
{
  ofstream writer;
  writer.open(Name.c_str(), ios_base::app);

  if ( Line_Started == false )
  {
    if ( Col_Date )
      writer << "Date,";
    if ( Col_Time )
      writer << "Time,";
    Line_Started = true;
  }
  writer.close();
}

//*******************************************************************************************
void DataLogger::Add_Title(const string field)
{
  ofstream writer;

  ChecktoAddTimeTitle();
  writer.open(Name.c_str(), ios_base::app);
  if ( Line_Started == true )
    writer << ",";
  writer << field.c_str();
  writer.flush();
  writer.close();
  Line_Started = true;
}

//*******************************************************************************************
void DataLogger::Add_Var(const string field)
{
  ofstream writer;

  ChecktoAddTime();
  writer.open(Name.c_str(), ios_base::app);

  if ( Line_Started == true )
    writer << ",";

  writer << "\"" << field << "\"";
  writer.flush();
  writer.close();
  Line_Started = true;
}

//*******************************************************************************************
void DataLogger::Add_Var(const int field)
{
  ofstream writer;

  ChecktoAddTime();
  writer.open(Name.c_str(), ios_base::app);
  if ( Line_Started == true )
    writer << ",";
  writer << field;
  writer.flush();
  writer.close();
  Line_Started = true;
}

//*******************************************************************************************
void DataLogger::Add_Var(const float field)
{
  ofstream writer;

  ChecktoAddTime();
  writer.open(Name.c_str(), ios_base::app);
  if ( Line_Started == true )
    writer << ",";

  writer << field;
  writer.close();
  Line_Started = true;
}

//*******************************************************************************************
void DataLogger::RecordLine(void)
{
  ofstream writer;

  ChecktoAddTime();

  writer.open(Name.c_str(), ios_base::app);
  writer << "\r\n";
  writer.flush();
  writer.close();
  Line_Started = false;
}


//*******************************************************************************************
//*******************************************************************************************

