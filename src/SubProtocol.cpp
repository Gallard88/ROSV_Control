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
static const struct Command CmdList[] = {
  { "SetPos",         true, true,   SetPos         },
  { "SetVel",         true, true,   SetVel         },
  { "SetControlMode", true, true,   SetControlMode },
  { "CamStart",       true, true,   CamStart       },
  { "CamStop",        true, true,   CamStop        },
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
SubProtocol::SubProtocol(int control_port)
{
  Control_Server = new TcpServer(control_port);
  ConProt = new ControlProtocol();
	update = 0;
}

// *******************************************************************************************
SubProtocol::~SubProtocol()
{
  delete ConProt;
  delete Control_Server;
}

// *******************************************************************************************
void SubProtocol::AddModule(const string & name, CmdModule *mod)
{
	struct Modules new_module;

	new_module.Name = name;
	new_module.module = mod;
	Modules.push_back(new_module);
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
		time_t current = time(NULL);
		if ((current - update) > 2) {
			update = current;
			if ( fp >= 0 ) {
				for ( size_t i = 0; i < Modules.size(); i ++ ) {
					ConProt->Write(fp, Modules[i].module->GetData());
				}
			}
		}
  }
  FD_SET(fp, &readfs);
  MAX_FP(fp, max_fp);


		if ( select(max_fp+1, &readfs, NULL, NULL, &to) > 0 ) {
    if ( ConProt->IsControlSourceConnected() == true ) {

      if ( FD_ISSET(ConProt->GetControlFileDescriptor(), &readfs)) {
        ConProt->GetControlData();
      }
    } else {

      if ( FD_ISSET(Control_Server->GetFp(), &readfs)) {

        src = Control_Server->Listen();
        if ( src != NULL ) {
          ConProt->AddControlSource(src);
					for ( size_t i = 0; i < Modules.size(); i ++ ) {
						ConProt->Write(fp, Modules[i].module->GetConfigData());
					}
          return;
        }
      }
    }
  }
  else
    return;
/*
  string arg;
  const struct Command *cmdPtr;
  while ( (cmdPtr = ConProt->GetCmds(CmdList, &arg, &fp)) != NULL ) {
    char buf[1024];

  }
  */
}

//*******************************************************************************************
//*******************************************************************************************

