/*
 ControlProtocol ( http://www.github.com/Gallard88/ROSV_Control )
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

/* ======================== */
using namespace std;

/* ======================== */
#include <stdio.h>
#include <string.h>

#include "ControlProtocol.h"

/* ======================== */
ControlProtocol::ControlProtocol(void)
{
  Controller = NULL;
  Observer = NULL;
}

/* ======================== */
ControlProtocol::~ControlProtocol(void)
{
  delete Controller;
  delete Observer;
  Controller = NULL;
  Observer = NULL;
}

/* ======================== */
void ControlProtocol::AddControlSource(DataSource *src)
{
	Controller = src;
}

/* ======================== */
bool ControlProtocol::IsControlSourceConnected(void)
{
	return Controller != NULL;
}

/* ======================== */
int ControlProtocol::GetControlFileDescriptor(void)
{
	if ( Controller == NULL )
		return -1;
	return Controller->GetFd();
}

/* ======================== */
void ControlProtocol::GetControlData(void)
{
	if ( Controller == NULL )
		return ;
	try {
  	  Controller->ReadData();
	} catch (int ex) {
		delete Controller;
		Controller = NULL;
	}
}

/* ======================== */
void ControlProtocol::AddObserverSource(DataSource *src)
{
  Observer = src;
}

/* ======================== */
int ControlProtocol::GetNumberOfObservers(void)
{
	if ( Observer == NULL )
		return 0;
	return 1;
}

/* ======================== */
int ControlProtocol::GetObserverFileDescriptor(void)
{
	if ( Observer == NULL )
		return -1;
	return Observer->GetFd();
}

/* ======================== */
void ControlProtocol::GetObserverData(void)
{
	if ( Observer == NULL )
		return ;
	try {
  	  Observer->ReadData();
	} catch (int ex) {
		delete Observer;
		Observer = NULL;
	}
}

/* ======================== */
void ControlProtocol::Write(int fd, string msg)
{
	this->Write(fd, msg.c_str());
}

/* ======================== */
void ControlProtocol::Write(int fd, const char *msg)
{
	if ( fd == Controller->GetFd() ) {
		Controller->WriteData(msg);
		Controller->WriteData("\r\n");
	} else {
		Observer->WriteData(msg);
		Observer->WriteData("\r\n");
	}
}

/* ======================== */
const struct Command *ControlProtocol::GetCmds(const struct Command *list, string *arg, int *fp)
{
	const struct Command *listPtr = list;
	string line;

	int length = 0;

	if ( Controller != NULL ) {
		length = Controller->ReadLine(&line);
		*fp = Controller->GetFd();
	}

	if (( length <= 0 ) && ( Observer != NULL )) {
		length = Observer->ReadLine(&line);
		*fp = Observer->GetFd();
	}

	if ( length > 0 )
	{
		// split into cmd and arg.
		size_t x = line.find_first_of("=:");
		if ( x != string::npos ) {
			*arg = line.substr((size_t) x+1, line.size());
			line.erase((size_t)x, line.size());
		}

		while ( listPtr->cmd != NULL )
		{
			if ( strcmp(listPtr->cmd, line.c_str()) == 0 )
				return listPtr;
			listPtr++;
		}
	}
	return NULL;
}

/* ======================== */
/*
void ControlProtocol::Run(const struct timeval *timeout)
{
  if ( DSrc != NULL ) {
    string line, arg;

    DSrc->Run(timeout);
    while ( DSrc->ReadLine(&line) != 0) {
      // split into cmd and arg.
      size_t x = line.find_first_of("=:");
      if ( x != string::npos ) {
        arg = line.substr((size_t) x+1, line.size());
        line.erase((size_t)x, line.size());
      }
      // look up command
      for ( int j = 0; j < (int)VecCallBack.size(); j ++ ) {
        CallBack c;

        c = VecCallBack[j];
        if ( line.compare(c.cmd) == 0 ) {
          (c.func)(DSrc->GetFd(), arg);
        }
      }
    }
  }
}
*/
/* ======================== */
/*
void ControlProtocol::AddCallback(const char *cmd, CmdCallback func)
{
  CallBack callback;

  callback.cmd = cmd;
  callback.func = func;

  VecCallBack.push_back(callback);
}
*/
/* ======================== */
/*
void ControlProtocol::SendMsg(const string msg)
{
  if ( DSrc != NULL )
    DSrc->WriteData(msg);
}
*/
/* ======================== */

/* ======================== */
/* ======================== */
