/*
 SubProtocol ( http://www.github.com/Gallard88/ROSV_Control )
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sstream>
#include <syslog.h>
#include <ctype.h>


#include "SubProtocol.h"

// *******************************************************************************************
using namespace std;

// *******************************************************************************************
// Protocol commands, version 0.01
// Write Commands
#define	SetPos					1
#define	SetVel					2
#define	SetControlMode	3
#define	CamStart				4
#define	CamStop					5
#define	LightToggle			6

// Read commands
#define	GetRealPos			7
#define	GetTargetPos		8
#define	GetRealVel			9
#define	GetTargetVel		10
#define	GetDiveTime			11
#define	GetVoltage			12
#define	GetTemp					13

// *******************************************************************************************
const struct Command CmdList[] = {
  { "SetPos",         true, true,   SetPos         },
  { "SetVel",         true, true,   SetVel         },
  { "SetControlMode", true, true,   SetControlMode },
  { "CamStart",       true, true,   CamStart       },
  { "CamStop",        true, true,   CamStop        },
  { "LightToggle",    true, true,   LightToggle    },
  { "GetRealPos",     true, false,  GetRealPos     },
  { "GetTargetPos",   true, false,  GetTargetPos   },
  { "GetRealVel",     true, false,  GetRealVel     },
  { "GetTargetVel",   true, false,  GetTargetVel   },
  { "GetDiveTime",    true, false,  GetDiveTime    },
  { "GetVoltage",     true, false,  GetVoltage     },
  { "GetTemp",        true, false,  GetTemp        },
  // indicates end of the list.
  { NULL,             false, false, 0              }
};

// *******************************************************************************************
SubProtocol::SubProtocol(int control_port, int observer_port)
{
  Control_Server = new TcpServer(control_port);
//  Observe_Server = new TcpServer(observer_port);
  ConProt = new ControlProtocol();
  this->Cam = NULL;
  this->Light = NULL;
  this->SCon = NULL;
}

// *******************************************************************************************
SubProtocol::~SubProtocol()
{
  delete ConProt;
  delete Control_Server;
//  delete Observe_Server;
}

// *******************************************************************************************
void SubProtocol::AddCameraManager(CameraManager *cam)
{
  this->Cam = cam;
}

// *******************************************************************************************
void SubProtocol::AddLightManager(LightManager *light)
{
  this->Light = light;
}

// *******************************************************************************************
void SubProtocol::AddSubControl(SubControl *scon)
{
  this->SCon = scon;
}

// *******************************************************************************************
#define MAX_FP(a, b)	b = (a > b)? a : b
// *******************************************************************************************
void SubProtocol::Run(const struct timeval *timeout)
{
  DataSource *src;
  fd_set readfs;
  int fp;
  int max_fp = 0;
  struct timeval to = *timeout;

  FD_ZERO(&readfs);
  if ( ConProt->IsControlSourceConnected() == false ) {
    fp  = Control_Server->GetFp();
  } else {
    fp = ConProt->GetControlFileDescriptor();
  }
  FD_SET(fp, &readfs);
  MAX_FP(fp, max_fp);
/*
  if ( ConProt->GetNumberOfObservers() == false ) {
    fp  = Observe_Server->GetFp();
  } else {
    fp = ConProt->GetControlFileDescriptor();
  }
  FD_SET(fp, &readfs);
  MAX_FP(fp, max_fp);
*/
  if ( select(max_fp+1, &readfs, NULL, NULL, &to) > 0 ) {
    if ( ConProt->IsControlSourceConnected() == true ) {

      if ( FD_ISSET(ConProt->GetControlFileDescriptor(), &readfs)) {
        ConProt->GetControlData();
			}
    } else {

      if ( FD_ISSET(Control_Server->GetFp(), &readfs)) {

        src = Control_Server->Listen();
        if ( src != NULL )
          ConProt->AddControlSource(src);
				return;
      }
    }
/*
    if ( ConProt->GetNumberOfObservers() != 0 ) {

      if ( FD_ISSET(ConProt->GetObserverFileDescriptor(), &readfs))
        ConProt->GetObserverData();
    } else {

      if ( FD_ISSET(Observe_Server->GetFp(), &readfs)) {

        src = Observe_Server->Listen();
        if ( src != NULL )
          ConProt->AddObserverSource(src);
      }
    }
*/
  }
  else
    return;

  string arg;
  const struct Command *cmdPtr;
  while ( (cmdPtr = ConProt->GetCmds(CmdList, &arg, &fp)) != NULL ) {
    char buf[1024];

    switch ( cmdPtr->func_number ) {
      // ---------------------------------------------------------------
// write commands
    case SetPos:
      SCon->SetTargetPos(ParseBearing(arg));
      printf("SetPos\n");
      break;

    case SetVel:
      SCon->SetTargetVel(ParseBearing(arg));
      //printf("SetVel\n");
      break;

    case LightToggle:		// Write
      Light->Toggle();
      break;

    case CamStart:			// Write
      Cam->Start();
      printf("Cam Start\n");
      break;

    case CamStop:			// Write
      Cam->Stop();
      printf("Cam Stop\n");
      break;

      // ---------------------------------------------------------------
// Read commands
    case GetRealPos:
      SendBearings(cmdPtr, fp, INS_GetPosition());
      printf("GetRealPos\n");
      break;

    case GetRealVel:
      SendBearings(cmdPtr, fp, INS_GetVelocity());
      //printf("GetRealVel\n");
      break;

    case GetTargetPos:
      SendBearings(cmdPtr, fp, SCon->Position );
      printf("GetTargetPos\n");
      break;

    case GetTargetVel:
      SendBearings(cmdPtr, fp, SCon->Velocity );
      break;

    case GetVoltage:
      sprintf(buf, "%s: %f\r\n", cmdPtr->cmd, PWM_GetVoltage());
      ConProt->Write(fp, buf);
      break;

    case GetTemp:
      sprintf(buf, "%s: %f\r\n", cmdPtr->cmd, PWM_GetTemp());
      ConProt->Write(fp, buf);
      break;

    case GetDiveTime:
      sprintf(buf, "%s: 0\r\n", cmdPtr->cmd);
      ConProt->Write(fp, buf);
      break;

      // ---------------------------------------------------------------
      // ---------------------------------------------------------------
    case SetControlMode:	// Write
    default :
      if ( cmdPtr->cmd)
        printf("Unknown Cmd: %s\n", cmdPtr->cmd);
      break;
    }
  }
}

// *******************************************************************************************
char *SkipSpace(char *buf)
{
  while ( *buf && isspace(*buf))
    buf++;
  return buf;
}

// *******************************************************************************************
char *SkipChars(char *buf)
{
  while ( *buf && !isspace(*buf))
    buf++;
  return buf;
}

// *******************************************************************************************
INS_Bearings SubProtocol::ParseBearing(string data)
{
  INS_Bearings bear;

  char *ptr = (char *)data.c_str();

  ptr = SkipSpace(ptr);
  bear.x = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  bear.y = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  bear.z = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  bear.roll = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  bear.pitch = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  bear.yaw = ::atof(ptr);
  return bear;
}

// *******************************************************************************************
string SubProtocol::ParseBearing(INS_Bearings data)
{
  stringstream ss;

  ss.clear();
  ss << data.x << ", ";
  ss << data.y << ", ";
  ss << data.z << ", ";
  ss << data.roll << ", ";
  ss << data.pitch << ", ";
  ss << data.yaw;

  string msg(ss.str());
  return msg;
}

//*******************************************************************************************
void SubProtocol::SendBearings(const struct Command *cmdPtr, int fd, INS_Bearings data)
{
  string msg(cmdPtr->cmd);
  msg += ": ";
  msg += ParseBearing(data);
  ConProt->Write(fd, msg);
}

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************

