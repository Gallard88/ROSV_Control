
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
// *******************************************************************************************
#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "CameraManager.h"

#define CAMMAN_SC_SIZE 4096

// *******************************************************************************************
CameraManager::CameraManager(const char *filename)
{
  StartTime = 0;

  JSON_Value *val = json_parse_file(filename);
  int rv = json_value_get_type(val);

  if ( rv != JSONObject ) {
    syslog(LOG_EMERG, "Camera: JSON Parse file failed\n");
    json_value_free (val);
    return;
  }

  JSON_Object *settings = json_value_get_object(val);
  if ( settings == NULL ) {
    syslog(LOG_EMERG, "Camera: Settings == NULL\n");
    json_value_free (val);
    return;
  }

  const char *ptr = json_object_get_string (settings, "Start");
  if ( ptr != NULL ) {
    asprintf(&StartSc, "%s",  ptr);
  } else {
    syslog(LOG_EMERG, "\"Start\" script not found");
  }

  ptr = json_object_get_string (settings, "Stop");
  if ( ptr != NULL ) {
    asprintf(&StopSc, "%s",  ptr);
  } else {
    syslog(LOG_EMERG, "\"Stop\" script not found");
  }
  json_value_free (val);
}

// *******************************************************************************************
CameraManager::~CameraManager()
{
  if ( StartSc != NULL )
    free(StartSc);
  if ( StopSc != NULL )
    free(StopSc);
}

// *******************************************************************************************
void CameraManager::Start(const char *ip)
{
//  pid_t pid = fork();

//  if ( pid == 0 ) {
  char *cmd;
  asprintf(&cmd, "%s %s &", StartSc, ip);
  syslog(LOG_EMERG, "Start Script:= %s", cmd);
  if ( system(cmd) < 0 ) {
    syslog(LOG_EMERG,"CameraManager: exec failed");
  }
  free(cmd);
//    exit(-1);
//  } else  if ( pid < 0 ) {
//    syslog(LOG_EMERG, "CameraManager: Failed to fork");
//    exit(-1);
//  }
  StartTime = time(NULL);
}

// *******************************************************************************************
long CameraManager::DiveTime(void)
{
  if ( StartTime ) {
    return time(NULL) - StartTime;
  } else {
    return 0;
  }
}

// *******************************************************************************************
void CameraManager::Stop(void)
{
  StartTime = 0;
}

// *******************************************************************************************
void CameraManager::Update(const char *packet, JSON_Object *msg)
{
  syslog(LOG_EMERG, "Msg\n");
  const char *rec_type = json_object_get_string(msg, "RecordType");
  if ( rec_type == NULL ) {
    return;
  }
  if ( strcmp(rec_type, "Start") == 0 ) {
    this->Start(json_object_get_string(msg, "IP"));
    syslog(LOG_EMERG, "Camera Start\n");
  }  else {
    syslog(LOG_EMERG, "Cam record: %s\n", rec_type);
  }
}

// *******************************************************************************************
const string CameraManager::GetData(void)
{
  char power[10];
  string msg("\"RecordType\": \"Update\", ");
  int diff = 0;

  if ( StartTime != 0 ) {
    diff = time(NULL) - StartTime;
  }


  msg += "\"Chanels\":[ ";
  msg += " {\"Name\": \"Primary\",\"Max\":100, \"Min\":0, \"Value\": ";
  sprintf(power, "%d", diff );
  msg += string(power);
  msg += "}";
  msg += " ] ";
  return msg;

}

// *******************************************************************************************
// *******************************************************************************************

