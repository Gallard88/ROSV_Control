/* ======================== */
#ifndef __CMDPROC_H__
#define __CMDPROC_H__

/* ======================== */
#include <string>
#include <vector>

/* ======================== */
typedef void (*CmdProc_Func)(string arg);

struct Cmd_Callback {
	string cmd;
	CmdProc_Func func;
};

/* ======================== */
class Cmdproc
{
private:
	vector<struct Cmd_Callback> Callback_List;

public:

	int AddCallBack(const string cmd, CmdProc_Func callback);
	int ProcessLine(string buf);

};

/* ======================== */
/* ======================== */
#endif
