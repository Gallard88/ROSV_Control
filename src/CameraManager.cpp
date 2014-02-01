
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
    strncpy(StartSc, ptr, CAMMAN_SC_SIZE);
  } else {
    syslog(LOG_EMERG, "\"Start\" script not found");
  }

  ptr = json_object_get_string (settings, "Stop");
  if ( ptr != NULL ) {
    strncpy(StopSc, ptr, CAMMAN_SC_SIZE);
  } else {
    syslog(LOG_EMERG, "\"Stop\" script not found");
  }
  json_value_free (val);
}

// *******************************************************************************************
void CameraManager::Start(const char *ip)
{
/*
  char cmd[CAMMAN_SC_SIZE*2];
  pid_t pid = fork();

  if ( pid == 0 ) {
    sprintf(cmd, "%s %s", StartSc, ip);
    if ( system(cmd) < 0 ) {
      syslog(LOG_EMERG,"CameraManager: exec failed");
    }
    exit(-1);
  } else  if ( pid < 0 ) {
    syslog(LOG_EMERG, "CameraManager: Failed to fork");
    exit(-1);
  }
*/
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
const string CameraManager::GetConfigData(void)
{
	return this->GetData();
}
// *******************************************************************************************
void CameraManager::Update(const string & msg)
{
}

// *******************************************************************************************
const string CameraManager::GetData(void)
{
	char msg[100];
	int diff = 0;

	if ( StartTime != 0 ) {
		diff = time(NULL) - StartTime;
	}

	sprintf(msg,"{ \"Module\": \"CameraData\", \"Time\": %d }", diff);
	return string(msg);
}

// *******************************************************************************************
// *******************************************************************************************

