
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
#include "CameraManager.h"
#include "EventMessages.h"
#include "MsgQueue.h"
#include "VideoStream.h"

using namespace std;

static EventMsg *Msg;

// *******************************************************************************************
CameraManager::CameraManager(VideoStreamer *video):
  Video(video)
{
  MQue = new MsgQueue("Camera", true);
  if ( Msg == NULL ) {
    Msg = EventMsg::Init();
  }
}

// *******************************************************************************************
CameraManager::~CameraManager()
{
}

MsgQueue *CameraManager::GetQueue(void)
{
  return MQue;
}

// *******************************************************************************************
void CameraManager::SendData(void)
{
  string msg;
  char buffer[256];

  snprintf(buffer,sizeof(buffer), "\"Port\": %u, ", Video->Get_Port());
  msg += string(buffer);

  snprintf(buffer,sizeof(buffer), "\"Width\": %u,", Video->Get_VideoWidth());
  msg += string(buffer);

  snprintf(buffer,sizeof(buffer), "\"Height\": %u,", Video->Get_VideoHeight());
  msg += string(buffer);

  snprintf(buffer,sizeof(buffer), "\"Frame\": %u,", Video->Get_VideoFramerate());
  msg += string(buffer);

  snprintf(buffer,sizeof(buffer), "\"Dur\": %u,", Video->Get_VideoDuration());
  msg += string(buffer);

  msg += "\"Client\": \"";
  msg += Video->GetClientName();
  msg += "\" ";

  MQue->Send("StreamInfo", msg);
}

// *******************************************************************************************
void CameraManager::Run_Task(void)
{
  if ( Video->isRecording() || MQue->IsBroadcast() ) {
    SendData();
  }
}

// *******************************************************************************************
// *******************************************************************************************

