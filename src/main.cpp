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

#include "DataSource.h"
#include "PowerManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "ControlProtocol.h"
#include "TcpServer.h"
#include "SubControl.h"
#include "SubProtocol.h"

/* ======================== */
/* ======================== */
#define RUN_INTERVAL  (100*1000)	//  10 ms
#define ALAM_INT      (50*1000)	//  50 ms

/* ======================== */
static const char prop_file[] = "/etc/ROSV_Motors.json";
const struct timeval system_time = { 0 , RUN_INTERVAL};
volatile bool Run_Control;

/* ======================== */
PWM_Con_t PwmModule;
TcpServer        *Listner;
SubControl       *MotorControl;
SubProtocol      *SubProt;
LightManager     *LightMan;
CameraManager    *CamMan;
PowerManager     *Power;

/* ======================== */
/* ======================== */
void Alarm_Wakeup (int i)
{
  Run_Control = true;
  LightMan->Run();
  Power->Run();
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
  openlog("ROSV_Control", LOG_PID, LOG_USER);
  syslog(LOG_EMERG, "Starting program");

  SignalHandler_Setup();
  atexit(System_Shutdown);

  /* --------------------------------------------- */
  // connect to other external systems
  PwmModule = PWM_Connect();
  if ( PwmModule == NULL ) {
    printf("PWM_Connect() failed\n");
    syslog(LOG_EMERG, "PWM_Connect() failed\n");
    return -1;
  }

  /* --------------------------------------------- */
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
  Listner = new TcpServer(8090);

  MotorControl = new SubControl("motors.json");
  MotorControl->Pwm = PwmModule;

  LightMan = new LightManager("lighting.json");
  LightMan->Pwm = PwmModule;

  Power = new PowerManager("power.json");
  Power->Pwm = PwmModule;

  CamMan = new CameraManager("camera.json");

  SubProt = new SubProtocol();
  SubProt->Pwm = PwmModule;

  SubProt->AddModule("Light", (CmdModule *) LightMan);
  SubProt->AddModule("Power", (CmdModule *) Power);
  SubProt->AddModule("Camera", (CmdModule *) CamMan);
  SubProt->AddModule("Motor", (CmdModule *) MotorControl);

  /* --------------------------------------------- */
  cout << "Starting Main Program" << endl;
  while (1) {
    // read data from connected clients.
    SubProt->Run(system_time);

    if ( Run_Control == true) {
      Run_Control = false;
      MotorControl->Run();
    }
  }
  return 0;
}

/* ======================== */
/* ======================== */
