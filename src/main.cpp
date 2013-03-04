/* ======================== */
using namespace std;

#include <iostream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include "TcpServer.h"
#include "CmdStream.h"
#include "PWM_Con.h"
#include "PowerMonitor.h"
#include "LightManager.h"
#include "DepthManager.h"
#include "PositionControl.h"

/* ======================== */
#define RUN_INTERVAL 100000	// 100 ms

/* ======================== */
const char prop_file[] = "./ROSV_Motors.json";
const struct timeval system_time = { 0 , RUN_INTERVAL};

/* ======================== */
volatile bool Run_Control;

/* ======================== */
TcpServer Listner(1);
PWM_Con Pwm;
PowerMonitor *PowerMon;
LightManager *Lighting;
PositionControl *Position;
DepthManager *Depth;

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
}

/* ======================== */
int main (int argc, char *argv[])
{
  JSON_Object *settings;
  JSON_Value *val;
  int rv;

  Setup_SignalHandler();
  atexit(System_Shutdown);

  // load paramaters
  val = json_parse_file(prop_file);
  rv = json_value_get_type(val);
  if ( rv != JSONObject )
  {
    printf("System didn't work %d\n", rv);
    return -1;
  }

  settings = json_value_get_object(val);
  if ( settings == NULL )
  {
    printf("Settings == NULL\n");
    return -1;
  }

  // open sub-modules
  PowerMon = new PowerMonitor(&Pwm);
  Lighting = new LightManager(settings, &Pwm);
  Position = new PositionControl(settings, &Pwm );
  Depth = new DepthManager(settings, &Pwm );
  Depth->Enable();

  // open TCP server
  if ( Listner.Connect(8090) < 0)
  {
    cout << "Listner Failed" << endl;
    return -1;
  }


  // set up timer signal and signal handler
  alarm_wakeup(0);

//  Listner.CheckNewConnetions();

  cout << "Starting Main Program" << endl;

  while (1)
  {
    struct timeval timeout = system_time;
    string line;

    // read data from connected clients.
    Listner.Run(&timeout);
    while ( Listner.ReadLine(&line) > 0 )
    {
      cout << line;
    }

    if ( Run_Control == true)
    {
      Run_Control = false;
      PowerMon->Run();
      Depth->Run();
      Position->Run();
    }
  }
  return 0;
}

/* ======================== */
/* ======================== */
