
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
#include <cstring>

#include "CameraManager.h"
#include "EventMessages.h"
#include "MsgQueue.h"

using namespace std;

#define CAMMAN_SC_SIZE 4096

static EventMsg *Msg;

// *******************************************************************************************
CameraManager::CameraManager(const char *filename)
{
  MQue = new MsgQueue("Camera", true);
  if ( Msg == NULL ) {
    Msg = EventMsg::Init();
  }

  JSON_Value *val = json_parse_file(filename);
  int rv = json_value_get_type(val);

  if ( rv != JSONObject ) {
    Msg->Log(EventMsg::ERROR,"Camera: JSON Parse file failed");
    json_value_free (val);
    return;
  }

  JSON_Object *settings = json_value_get_object(val);
  if ( settings == NULL ) {
    Msg->Log(EventMsg::ERROR,"Camera: Settings == NULL");
    json_value_free (val);
    return;
  }

  const char *ptr = json_object_get_string (settings, "Start");
  if ( ptr != NULL ) {
    asprintf(&StartSc, "%s",  ptr);
  } else {
    Msg->Log(EventMsg::ERROR,"\"Start\" script not found");
  }

  ptr = json_object_get_string (settings, "Stop");
  if ( ptr != NULL ) {
    asprintf(&StopSc, "%s",  ptr);
  } else {
    Msg->Log(EventMsg::ERROR,"\"Stop\" script not found");
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

MsgQueue *CameraManager::GetQueue(void)
{
  return MQue;
}

// *******************************************************************************************
void CameraManager::Start(const char *ip)
{
//  pid_t pid = fork();

//  if ( pid == 0 ) {
  char *cmd;
  asprintf(&cmd, "%s %s &", StartSc, ip);
  Msg->Log(EventMsg::ERROR,"Start Script:= %s", cmd);
  if ( system(cmd) < 0 ) {
    Msg->Log(EventMsg::ERROR,"CameraManager: exec failed");
  }
  free(cmd);
//    exit(-1);
//  } else  if ( pid < 0 ) {
//    exit(-1);
//  }
}

// *******************************************************************************************
long CameraManager::DiveTime(void)
{
  return 0;
}

// *******************************************************************************************
void CameraManager::Stop(void)
{
}

// *******************************************************************************************
void CameraManager::SendData(void)
{
  char power[10];
  string msg;
  int diff = 0;

  msg += "\"Channels\":[ ";
  msg += " {\"Name\": \"Primary\",\"Max\":100, \"Min\":0, \"Value\": ";
  sprintf(power, "%d", diff );
  msg += string(power);
  msg += "}]";
  MQue->Send("Update", msg);

}

// *******************************************************************************************
void CameraManager::Run_Task(void)
{
}

// *******************************************************************************************
// *******************************************************************************************

