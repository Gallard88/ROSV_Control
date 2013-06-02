


using namespace std;

#include "ProtocolDecoder.h"

#include <iostream>
ProtocolDecoder::ProtocolDecoder(void)
{

}

ProtocolDecoder::~ProtocolDecoder(void)
{
  CmdList.clear();
  Sources.clear();

}

int ProtocolDecoder::AddSource(int fp)
{
  struct DataSource data;
  int key = NextKey++;

  data.fp = fp;
  data.time = 0;
  data.data.clear();
  data.call = NULL;

  Sources.insert( std::pair<int, struct DataSource>(key, data));
  return key;
}

void ProtocolDecoder::RemoveSource(int key)
{
  Sources.erase(key);
}

void ProtocolDecoder::AddErrorCallback(int key, ErrCallBack err)
{
  Sources[key].call = err;
}

void ProtocolDecoder::SetErrorTimeout(int key, int milliseconds)
{
  Sources[key].timeout = milliseconds;
}

/* ---------------------------------------------------- */
int ProtocolDecoder::AddCmd(const char *cmd, CmdCallBack func)
{
  struct CmdHandlers handle;

  handle.cmd = string(cmd);
  handle.func = func;
  return 0;
}

void ProtocolDecoder::BroadCast(string msg)
{
}

void ProtocolDecoder::Run(const struct timeval *timeout)
{
#define READ_BUF_SIZE	4096
  char buf[READ_BUF_SIZE+1];
  struct timeval to = *timeout;
  fd_set readfs;
  int max_sock = 0;

  FD_ZERO(&readfs);

  for (std::map<int,struct DataSource>::iterator it=Sources.begin(); it!=Sources.end(); ++it)
  {
    struct DataSource d = it->second;
    FD_SET( d.fp ,&readfs);
    if ( max_sock > d.fp )
      d.fp = max_sock;
  }
  if ( select(max_sock+1, &readfs, NULL, NULL, &to) < 0)
    return;

  for (std::map<int,struct DataSource>::iterator it=Sources.begin(); it!=Sources.end(); ++it)
  {
    struct DataSource d = it->second;
    if ( FD_ISSET(d.fp, &readfs) )
    {
      int rv = read(d.fp, buf, READ_BUF_SIZE);
      if ( rv < 0 )
      {// call error func
      }
      else
      {// process data
        buf[READ_BUF_SIZE] = 0;
        cout << "data read" << endl;
      }
    }
  }
}


