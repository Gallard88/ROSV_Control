/* ======================== */
#ifndef __CMD_STREAM__
#define __CMD_STREAM__
/* ======================== */
#include <map>
#include <string>
#include <vector>

/* ======================== */
typedef void cmd_func_t(const std:: string&);
typedef std::map<std::string, cmd_func_t*> stfnmap_t;

struct CS_Source_t
{
    int fd;
    string buffer;
};

/* ======================== */
class CmdStream
{
private:
    stfnmap_t CmdMap;
    vector<struct CS_Source_t> SrcVec;

    fd_set readfds;
    int max_sock;
//	#define SET_MAX_SOCK(x)	if( x > max_sock ) max_sock = x;

public:
    CmdStream();

    void AddSouce(int fd);
    void RemoveSource(int fd);

    void AddCmd(string cmd, cmd_func_t func);
    void RemoveCmd(string cmd);

    void Run(void);

};

/* ======================== */
/* ======================== */
#endif
