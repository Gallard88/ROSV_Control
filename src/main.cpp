/* ======================== */
using namespace std;

#include <iostream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sstream>
#include <syslog.h>

#include <PWM_Controller.h>

#include "TcpServer.h"
#include "CmdStream.h"
#include "PowerMonitor.h"
#include "LightManager.h"
#include "DepthManager.h"
#include "PositionControl.h"
#include "CmdProc.h"

/* ======================== */
#define RUN_INTERVAL 100000	// 100 ms

/* ======================== */
const char prop_file[] = "/etc/ROSV_Motors.json";
const struct timeval system_time = { 0 , RUN_INTERVAL};

/* ======================== */
volatile bool Run_Control;
bool Enable;

/* ======================== */
TcpServer Listner(1);
PowerMonitor *PowerMon;
LightManager *Lighting;
PositionControl *Position;
DepthManager *Depth;
Cmdproc *Cmd;

/* ======================== */
void alarm_wakeup (int i)
{
  struct itimerval tout_val;

  signal(SIGALRM,alarm_wakeup);

  tout_val.it_interval.tv_sec = 0;
  tout_val.it_interval.tv_usec = 0;
  tout_val.it_value.tv_sec = 0; // 10 seconds timer
  tout_val.it_value.tv_usec = RUN_INTERVAL;

  setitimer(ITIMER_REAL, &tout_val,0);
  Run_Control = true;
}

// *****************
void Run_CtrlC(int sig)
{
  exit(0);
}

// *****************
void Setup_SignalHandler(void)
{
  struct sigaction sig;

  // Install timer_handler as the signal handler for SIGVTALRM.
  memset (&sig, 0, sizeof (struct sigaction));
  sig.sa_handler = &Run_CtrlC;
  sigaction (SIGINT , &sig, NULL);
  sigaction (SIGTERM , &sig, NULL);
  sigaction (SIGABRT , &sig, NULL);
}

/* ======================== */
void System_Shutdown(void)
{
  delete PowerMon;
  delete Lighting;
  delete Position;
  delete Depth;
  syslog(LOG_EMERG, "System shutting down");
  closelog();
}

/* ======================== */
void Func_Forward(string arg)
{
  double temp = ::atof(arg.c_str()) / 100.0;

  cout << "Func_Forward: " << temp << endl;
  Position->Set_TargFwdVel(temp);
}

/* ======================== */
void Func_Sideward(string arg)
{
  double temp = ::atof(arg.c_str()) / 100.0;
  cout << "Func_Sideward: " << temp << endl;

  Position->Set_TargSwdVel(temp);
}

/* ======================== */
void Func_Turn(string arg)
{
  double temp = ::atof(arg.c_str()) / 100.0;
  cout << "Func_Turn: " << temp << endl;
  Position->Set_TargTurnVel(temp);
}

/* ======================== */
void Func_Depth(string arg)
{
  double temp = ::atof(arg.c_str()) / 100.0;

  cout << "Func_Depth: " << temp << endl;

	if ( Enable == false )
		temp = 0.0;	// disable depth
  Depth->SetDepthPower(temp);
}

/* ======================== */
void Func_Enable(string arg)
{
	Enable = true;
  syslog(LOG_EMERG, "System Enabled");
}

/* ======================== */
void Func_Disable(string arg)
{
	Enable = false;
  syslog(LOG_EMERG, "System Disabled");
}

/* ======================== */
void Setup_Callbacks(void)
{
  Cmd->AddCallBack("Forward", 		Func_Forward);
  Cmd->AddCallBack("Sideward", 		Func_Sideward);
  Cmd->AddCallBack("Turn", 				Func_Turn);
  Cmd->AddCallBack("Depth", 			Func_Depth);
  Cmd->AddCallBack("Disable", 		Func_Disable);
  Cmd->AddCallBack("Enable", 			Func_Enable);
}

/* ======================== */
int main (int argc, char *argv[])
{
  JSON_Object *settings;
  JSON_Value *val;
  int rv;
  string msg;
  stringstream ss;

	Enable = false;
  openlog("ROSV_Control", LOG_PID, LOG_USER);
  syslog(LOG_EMERG, "Starting program");

  Setup_SignalHandler();
  atexit(System_Shutdown);

  // load paramaters
  val = json_parse_file(prop_file);
  rv = json_value_get_type(val);
  if ( rv != JSONObject )
  {
    printf("JSON Parse file failed\n");
    return -1;
  }

  settings = json_value_get_object(val);
  if ( settings == NULL )
  {
    printf("Settings == NULL\n");
    return -1;
  }
  if ( PWM_Connect() < 0 )
  {
    printf("PWM_Connect() failed\n");
    return -1;
  }
  uid_t uid=getuid(), euid=geteuid();
  string path;
  if ((uid <= 0 ) || ( uid != euid))
  {
    /* We might have elevated privileges beyond that of the user who invoked
    * the program, due to suid bit. Be very careful about trusting any data! */
    path = "/var/log/";
    cout << "Operating as Root" << endl;
  } else {
    path = "./";
    cout << "Operating as User " << uid << endl;
  }

  // open sub-modules
  Cmd = new Cmdproc();
  PowerMon = new PowerMonitor(path);
  Lighting = new LightManager(settings);
  Position = new PositionControl(settings);
  Depth = new DepthManager(settings );
  Depth->SetDepthPower(0.0);

  Setup_Callbacks();

  // open TCP server
  if ( Listner.Connect(8090) < 0)
  {
    cout << "Listner Failed" << endl;
    return -1;
  }


  // set up timer signal and signal handler
  alarm_wakeup(0);

  cout << "Starting Main Program" << endl;
  while (1)
  {
    struct timeval timeout = system_time;
    string line;

    // read data from connected clients.
    Listner.Run(&timeout);
    line.clear();
    while ( Listner.ReadLine(&line) > 0 )
    {
      if ( line.size() > 0 )
      {
        Cmd->ProcessLine(line);
      }
    }

    if ( Run_Control == true)
    {
      Run_Control = false;
      PowerMon->Run();
      Depth->Run();
      Position->Run();

      // send data back to console.
      msg.clear();
      ss.str(std::string());
      ss << PWM_GetTemp();
      msg = "Temp=" + ss.str() + "\r\n";

      ss.str(std::string());
      ss << PWM_GetVoltage();
      msg += "Volt=" + ss.str() + "\r\n";

      ss.str(std::string());
      ss << PWM_GetCurrent();
      msg += "Current=" + ss.str() + "\r\n";

      Listner.SendMsg(msg);

    }
  }
  return 0;
}

/* ======================== */
/* ======================== */
