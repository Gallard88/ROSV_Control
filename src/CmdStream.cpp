/* ======================== */
using namespace std;
/* ======================== */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include "CmdStream.h"

#define SYSTEM_DELAY	10000
const struct timeval system_time = {0,SYSTEM_DELAY};
/* ======================== */

CmdStream::CmdStream()
{
  FD_ZERO(&readfds);
}

/* ======================== */
void CmdStream::AddSouce(int fd)
{
	string str;

	SrcMap[fd] = str;
	SET_MAX_SOCK(fd);
	FD_SET(fd, &readfds);
}

/* ======================== */
void CmdStream::RemoveSource(int fd)
{
	;
}

/* ======================== */
void CmdStream::AddCmd(string cmd, cmd_func_t func)
{
	CmdMap[cmd] = func;
}

/* ======================== */
void CmdStream::RemoveCmd(string cmd)
{
	;
}

/* ======================== */
void CmdStream::Run(void)
{
	select(max_sock+1, &readfds, NULL, NULL, NULL);	// wait indefinately
	for ( std::map<int, std::string>::iterator it = SrcMap.begin(); it != SrcMap.end(); it++ )
	{
		if ( FD_ISSET(DataSocket, &readfds) )
	}
}

/* ======================== */

/* ======================== */
/* ======================== */
