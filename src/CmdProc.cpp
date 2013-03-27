/* ======================== */
using namespace std;

/* ======================== */
#include <iostream>

#include "CmdProc.h"

/* ======================== */
int Cmdproc::AddCallBack(const string cmd, CmdProc_Func callback)
{
  Cmd_Callback new_element;

  new_element.cmd = cmd;
  new_element.func = callback;

  Callback_List.push_back(new_element);
  cout << "Call back added: " << new_element.cmd << endl;
  return 0;
}

/* ======================== */
int Cmdproc::ProcessLine(string data)
{
  string arg;
  {
    string cmd = data;

    size_t x = cmd.find_first_of("=:");
    if ( x != string::npos )
    {
      arg = cmd.substr((size_t) x+1, cmd.size());
      cmd.erase((size_t)x, cmd.size());
    }

    for ( int j = 0; j < (int)Callback_List.size(); j ++ )
    {
      struct Cmd_Callback c;

      c = Callback_List[j];
      if ( cmd.compare(c.cmd) == 0 )
      {
        (c.func)(arg);
      }
    }
  }
  return 0;
}

/* ======================== */
