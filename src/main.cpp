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
#include <ctype.h>

#include <PWM_Controller.h>
#include <INS_Data.h>

#include "DataSource.h"
//#include "DiveMonitor.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "ControlProtocol.h"
#include "TcpServer.h"
#include "SubControl.h"
#include "SubProtocol.h"
#include "SysSettings.h"

/* ======================== */
/* ======================== */
#define RUN_INTERVAL  (100*1000)	//  10 ms
#define ALAM_INT      (50*1000)	//  50 ms

/* ======================== */
//const char prop_file[] = "./ROSV_Motors.json";
const char prop_file[] = "/etc/ROSV_Motors.json";
const struct timeval system_time = { 0 , RUN_INTERVAL};
volatile bool Run_Control;

/* ======================== */
PWM_Con_t PwmModule;
SubControl       *MotorControl;
SubProtocol      *SubProt;
//DiveMonitor      *DiveMon;
LightManager     *LightMan;
CameraManager    *CamMan;

/* ======================== */
/* ======================== */
void Alarm_Wakeup (int i)
{
  Run_Control = true;
	LightMan->Run();
}

/* ======================== */
void SignalHandler_Setup(void)
{
  struct itimerval timeout;
  struct sigaction sig;

  // Install timer_handler as the signal handler for SIGVTALRM.
  memset (&sig, 0, sizeof (struct sigaction));
  sig.sa_handler = &exit;
  sigaction (SIGINT , &sig, NULL);
  sigaction (SIGTERM , &sig, NULL);
  sigaction (SIGABRT , &sig, NULL);

  memset (&sig, 0, sizeof (struct sigaction));
  sig.sa_handler = &Alarm_Wakeup;
  sigaction (SIGALRM , &sig, NULL);

  timeout.it_interval.tv_sec = 0;
  timeout.it_interval.tv_usec = ALAM_INT;
  timeout.it_value.tv_sec = 0;
  timeout.it_value.tv_usec = ALAM_INT;

  setitimer(ITIMER_REAL, &timeout,0);
  Run_Control = true;
}

/* ======================== */
void System_Shutdown(void)
{
  if ( MotorControl )
    delete MotorControl;
//  if ( DiveMon )
//    delete DiveMon;
  if ( LightMan )
    delete LightMan;
  if ( CamMan )
    delete CamMan;

  syslog(LOG_EMERG, "System shutting down");
  closelog();
}

/* ======================== */
/* ======================== */
int main (int argc, char *argv[])
{
  int rv;

  openlog("ROSV_Control", LOG_PID, LOG_USER);
  syslog(LOG_EMERG, "Starting program");

  SignalHandler_Setup();
  atexit(System_Shutdown);

  /* --------------------------------------------- */
  // load paramaters
  JSON_Value *val = json_parse_file(prop_file);
  rv = json_value_get_type(val);
  if ( rv != JSONObject ) {
    printf("JSON Parse file failed\n");
    syslog(LOG_EMERG, "JSON Parse file failed\n");
    return -1;
  }

  JSON_Object *settings = json_value_get_object(val);
  if ( settings == NULL ) {
    printf("Settings == NULL\n");
    return -1;
  }

  /* --------------------------------------------- */
  // connect to other external systems
  PwmModule = PWM_Connect();
  if ( PwmModule == NULL ) {
    printf("PWM_Connect() failed\n");
    syslog(LOG_EMERG, "PWM_Connect() failed\n");
    return -1;
  }

  if ( INS_Connect() < 0 ) {
    printf("INS_Connect() failed\n");
    syslog(LOG_EMERG, "INS_Connect() failed\n");
    return -1;
  }

  /* --------------------------------------------- */
  string path;
  uid_t uid=getuid(), euid=geteuid();

  if ((uid <= 0 ) || ( uid != euid)) {
    // We might have elevated privileges beyond that of the user who invoked
    // the program, due to suid bit. Be very careful about trusting any data!
    cout << "Operating as Root" << endl;
  } else {
    cout << "Operating as User " << uid << endl;
  }

  /* --------------------------------------------- */
  // open sub-modules

  MotorControl = new SubControl(settings);
	MotorControl->Pwm = PwmModule;

 // DiveMon = new DiveMonitor(settings, MotorControl);
//	DiveMon->Pwm = PwmModule;

  LightMan = new LightManager("lighting.json");
	LightMan->Pwm = PwmModule;

	CamMan = new CameraManager(settings);

  SubProt = new SubProtocol(8090);
	SubProt->Pwm = PwmModule;

	SubProt->AddModule("Light", (CmdModule *) LightMan);

  SubProt->AddCameraManager(CamMan);
  SubProt->AddSubControl(MotorControl);

  json_value_free(val);

  /* --------------------------------------------- */
  cout << "Starting Main Program" << endl;
  while (1) {
    // read data from connected clients.
    SubProt->Run(&system_time);

      if ( Run_Control == true) {
        Run_Control = false;

      MotorControl->Run();
//      DiveMon->Run();
      LightMan->Run();
    }
  }
  return 0;
}

/* ======================== */
/* ======================== */
