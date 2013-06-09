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

#include "ControlProtocol.h"

/* ======================== */
void ControlProtocol::AddDataSource(DataSource *src)
{
	DSrc = src;
}

/* ======================== */
void ControlProtocol::Run(const struct timeval *timeout)
{
	if ( DSrc != NULL ) {
		string line, arg;

		DSrc->Run(timeout);
		while ( DSrc->ReadLine(&line) != 0) {
			// split into cmd and arg.
			size_t x = line.find_first_of("=:");
			if ( x != string::npos )
			{
				arg = line.substr((size_t) x+1, line.size());
				line.erase((size_t)x, line.size());
			}
			// look up command
			for ( int j = 0; j < (int)VecCallBack.size(); j ++ )
			{
				CallBack c;

				c = VecCallBack[j];
				if ( line.compare(c.cmd) == 0 )
				{
					(c.func)(DSrc->GetFd(), arg);
				}
			}
		}
	}
}

/* ======================== */
void ControlProtocol::AddCallback(const char *cmd, CmdCallback func)
{
	CallBack callback;

	callback.cmd = cmd;
	callback.func = func;

	VecCallBack.push_back(callback);
}

/* ======================== */
void ControlProtocol::SendMsg(const string msg)
{
	if ( DSrc != NULL )
		DSrc->WriteData(msg);
}

/* ======================== */

/* ======================== */
/* ======================== */
