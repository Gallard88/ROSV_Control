
#include <cstring>
#include <cstdio>
#include <stdarg.h>
#include <mutex>
#include <ctime>
#include <sys/time.h>

#include "EventMessages.h"
#include "MsgQueue.h"

using namespace std;

static EventMsg * EventPtr = NULL;
std::mutex Mtx;

EventMsg::EventMsg():
  PrintSyslog(true), PrintTerminal(true),
  Filename(""), Num_Msgs(10)
{
  MQue = new MsgQueue("Messages", false);
}

EventMsg::~EventMsg()
{
}

MsgQueue *EventMsg::GetQueue(void)
{
  return MQue;
}

const char *EventText[] = {
  "Debug",  //  DEBUG,
  "Info",   //  INFO,
  "Notice", //  NOTICE,
  "Warn",   //  WARN,
  "Error"   //  ERROR
};

EventMsg *EventMsg::Init()
{
  if ( EventPtr == NULL ) {
    EventPtr = new EventMsg();
  }
  return EventPtr;
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
  char text [512];
  va_list args;

  FlagReady();

  va_start(args,fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm tm = *localtime(&tv.tv_sec);

  snprintf(text, sizeof(text), "%04d-%02d-%02d, %02d:%02d:%02d.%03d, %s, %s\r",
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec, (int)(tv.tv_usec / 1000),
           EventText[type], buf );

  Mtx.lock();
  if ( PrintTerminal == true ) {
    puts(text);
  }
  if ( Messages.size() >= Num_Msgs) {
    Messages.erase(Messages.begin());
  }
  Messages.push_back(std::string(text));

  FILE * fp = fopen(Filename.c_str(), "a" );
  if ( fp != NULL ) {
    fwrite(text, strlen(text), 1, fp);
    fputs("\n", fp);
    fclose(fp);
  } else {
    perror("open()");
  }
  Mtx.unlock();
}

/* ============================================ */
// Cmd Module
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
    if (( size > 1 ) && ( i < (size-1))) {
      msg += ", ";
    }
  }
  msg += " ]";
  return msg;
}

