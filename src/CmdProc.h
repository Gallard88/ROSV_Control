/* ======================== */
#ifndef __CMDPROC_H__
#define __CMDPROC_H__

/* ======================== */
#include <string>
#include <vector>

/* ======================== */

struct cmd_Callback {
	string cmd;
	void (*callback)(string arg);
};

/* ======================== */
class Cmdproc
{
private:
	vector<struct cmd_Callback> Callback_List;
	string Buffer;

public:

	int AddCallBack(struct cmd_Callback *callback);
	int Process(string buf);

};

/* ======================== */
/* ======================== */
#endif
