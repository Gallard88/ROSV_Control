/*   */
#ifndef __PROTOCOL_DECODER__
#define __PROTOCOL_DECODER__

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <string>
#include <map>
#include <vector>

typedef void (*ErrCallBack)(int fp);
typedef void (*CmdCallBack)(string args);

struct DataSource
{
  int fp;
  time_t time;
  int timeout;
  string data;
  ErrCallBack call;
};

struct CmdHandlers
{
  string cmd;
  CmdCallBack func;
};

/**
  invalid chars , : \r\n
  set, cmd: args\r\n
  get, cmd: args\r\n
  list, [set|get]\r\n
  Error, bad cmd\r\n
  Ok\r\n
**/

class ProtocolDecoder
{
public:
  ProtocolDecoder(void);
  ~ProtocolDecoder(void);

  /* Data source Functions */
  int AddSource(int fp);
  void RemoveSource(int key);
  void AddErrorCallback(int key, ErrCallBack err);
  void SetErrorTimeout(int key, int milliseconds);

  /* Cmd Handling Functions */
  int AddCmd(const char *cmd, CmdCallBack func);
  void BroadCast(string msg);
  void Run(const struct timeval* timeout);

private:
  void SendError(int fp, string cmd);
  void SendOK(int fp);

  std::map<int,struct DataSource> Sources;
  vector<struct CmdHandlers> CmdList;
  int NextKey;
};

#endif
