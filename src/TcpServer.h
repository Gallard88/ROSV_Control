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



public:

	TcpServer(int max_clients);

	void BuildSelectList(void);
	void CheckNewConnetions(void);

	int Connect(int port);
	void Run(struct timeval *timeout);

	int ReadLine(string *cmd, string *arg);
};

/* ======================== */
/* ======================== */
#endif
