/* ======================== */
using namespace std;

/* ======================== */
#include <iostream>

#include "CmdProc.h"

/* ======================== */
int Cmdproc::AddCallBack(struct cmd_Callback *callback)
{
		Callback_List.push_back(*callback);
		cout << "Call back added: " << callback->cmd << endl;
		return 0;
}

/* ======================== */
int Cmdproc::Process(string data)
{
	int found;

	Buffer += data;
	while ((found = Buffer.find_first_of("\r\n")) != string::npos)
	{

		string arg;
		string cmd = Buffer.substr(0, found);
		Buffer.erase(0, found);

		cout << "Cmd found: " << cmd << endl;

		size_t x = cmd.find_first_of("=:");
		if ( x != string::npos )
		{
			arg = cmd.substr((size_t) x+1, cmd.size());
			cmd.erase((size_t)x, cmd.size());
			cout << "Arg Built: " << arg << endl;
		}
		for ( int j = 0; j < (int)Callback_List.size(); j ++ )
		{
			struct cmd_Callback c;

			c = Callback_List[j];
			if ( cmd.compare(c.cmd) == 0 )
			{
				cout << "Cmd match :D" << endl;
				(c.callback)(arg);
			}
		}
	}
	return 0;
}

/* ======================== */
