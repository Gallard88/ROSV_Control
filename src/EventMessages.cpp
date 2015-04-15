
#include <cstring>
#include <syslog.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "EventMessages.h"

using namespace std;

static EventMsg * EventPtr = NULL;

EventMsg::EventMsg():
  PrintSyslog(true), PrintTerminal(true),
  Filename(""), Num_Msgs(10)
{
}

EventMsg::~EventMsg()
{
}

const char *EventText[] = {
  "Debug",  //  DEBUG,
  "Info",   //  INFO,
  "Notice", //  NOTICE,
  "Warn",   //  WARN,
  "Error"   //  ERROR
};

const int Syslog_Types[] = {
  LOG_DEBUG,   //  DEBUG,
  LOG_INFO,    //  INFO,
  LOG_NOTICE,  //  NOTICE,
  LOG_WARNING, //  WARN,
  LOG_EMERG    //  ERROR
};

EventMsg *EventMsg::Init()
{
  if ( EventPtr == NULL ) {
    EventPtr = new EventMsg();
  }
  return EventPtr;
}

void EventMsg::sendToSyslog(bool en)
{
  PrintSyslog = en;
}

void EventMsg::sendToTerminal(bool en)
{
  PrintTerminal = en;
}

void EventMsg::setFilename(const std::string & filename)
{
  Filename = filename;
}

void EventMsg::setLogDepth(int log_depth)
{
  Num_Msgs = log_depth;
}

void EventMsg::Log(EventMsg_t type, const char *fmt, ...)
{
  char buf [512];
  va_list args;

  va_start(args,fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  if ( PrintSyslog == true ) {
    syslog(Syslog_Types[type], buf);
  }
  if ( PrintTerminal == true ) {
    printf("%s, %s\n", EventText[type], buf);
  }
  if ( Messages.size() >= Num_Msgs) {
    Messages.erase(Messages.begin());
  }
  Messages.push_back(std::string(buf));
  int fp = open(Filename.c_str(), O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
  if ( fp >= 0 ) {
    write(fp, buf, strlen(buf));
    close(fp);
  }
}

/* ============================================ */
// Cmd Module
void EventMsg::Update(const char *packet, JSON_Object *msg)
{
}

const std::string EventMsg::GetData(void)
{
  string msg;
  size_t size = Messages.size();

  msg = "\"RecordType\": \"SystemMessages\", ";
  msg += "\"Messages\":[ ";

  for ( size_t i = 0; i < size; i ++ ) {
    msg += "\"";
    msg += Messages[i];
    msg += "\"";
    if (( i > 1 ) && ( i < (size-1))) {
      msg += ", ";
    }
  }
  msg += " ]";
  return msg;
}

