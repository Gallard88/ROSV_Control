/* ======================== */
#ifndef _TCPSERVER__
#define _TCPSERVER__

/* ======================== */
#include <string>
#include <vector>

struct TcpData
{
  int fd;
  string buffer;
};

/* ======================== */
class TcpServer
{
private:
  int listen_fd;
  int Max_Connections;

  vector<struct TcpData> Socket_Vec;
  fd_set readfs;

  void CheckNewConnetions(void);
  void Prune(void);


public:

  TcpServer(int max_clients);
  ~TcpServer(void);

  void BuildSelectList(void);

  int Connect(int port);
  void Run(struct timeval *timeout);
  void SendMsg(const string msg);
  void SendMsg(const char *data, int size);

  int ReadLine(string *data);
};

/* ======================== */
/* ======================== */
#endif
