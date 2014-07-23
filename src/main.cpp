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

#include <thread>

#include <PWM_Controller.h>

#include "DataSource.h"
#include "PowerManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "TcpServer.h"
#include "SubControl.h"
#include "SubProtocol.h"
#include "Logger.h"

/* ======================== */
/* ======================== */
#define RUN_INTERVAL  (50*1000)    // us
#define ALAM_INT      (50*1000)    // us

/* ======================== */
static const char prop_file[] = "/etc/ROSV_Motors.json";
const struct timeval system_time = { 0 , RUN_INTERVAL};
volatile bool Run_Control;

/* ======================== */
PWM_Con_t PwmModule;
TcpServer        *Listner;
volatile bool	 RunListner;
SubControl       *MotorControl;
SubProtocol      *SubProt;
LightManager     *LightMan;
CameraManager    *CamMan;
PowerManager     *Power;
Logger           *Log;

static thread ListenThread;

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
  RunListner = false;
//  ListenThread.join();
//  if ( Listner != NULL ) {
//    delete Listner;
//  }
  if ( Power != NULL ) {
    delete Power;
  }
  if ( MotorControl != NULL ) {
    delete MotorControl;
  }
  if ( LightMan != NULL ) {
    delete LightMan;
  }
  if ( CamMan != NULL ) {
    delete CamMan;
  }
  if ( SubProt != NULL ) {
    delete SubProt;
  }

  Log->RecordValue("ROSV_Control", "Shutdown", 1);
  syslog(LOG_NOTICE, "System shutting down");
  closelog();
}

/* ======================== */
static void ListenFunc(void)
{
  DataSource *new_src;

  while ( RunListner ) {
    new_src = Listner->Listen();
    if ( new_src != NULL ) {
      SubProt->AddSource(new_src);
    }
  }
}

/* ======================== */
int main (int argc, char *argv[])
{
  openlog("ROSV_Control", LOG_PID, LOG_USER);
  syslog(LOG_NOTICE, "Starting program");

  SignalHandler_Setup();
  atexit(System_Shutdown);

  /* --------------------------------------------- */
  // connect to other external systems
  PwmModule = PWM_Connect();
  if ( PwmModule == NULL ) {
    printf("PWM_Connect() failed\n");
    syslog(LOG_ALERT, "PWM_Connect() failed\n");
    return -1;
  }

  /* --------------------------------------------- */
  // open sub-modules
  Log = Logger::Init();
  Log->RecordValue("ROSV_Control","Start", 1);

  Listner = new TcpServer(8090);
  RunListner = true;
  std::thread ListenThread(ListenFunc);

  MotorControl = new SubControl("/etc/ROSV_Control/motors.json", PwmModule);

  LightMan = new LightManager("/etc/ROSV_Control/lighting.json");
  LightMan->Pwm = PwmModule;

  Power = new PowerManager("/etc/ROSV_Control/power.json");
  Power->Pwm = PwmModule;

  CamMan = new CameraManager("/etc/ROSV_Control/camera.json");

  SubProt = new SubProtocol();
  SubProt->Pwm = PwmModule;

  SubProt->AddModule("Light",  (CmdModule *) LightMan);
  SubProt->AddModule("Power",  (CmdModule *) Power);
  SubProt->AddModule("Motor",  (CmdModule *) MotorControl);
  SubProt->AddModule("Camera", (CmdModule *) CamMan);

  /* --------------------------------------------- */
  while (1) {
    // read data from connected clients.
    SubProt->Run(system_time);

    if ( Run_Control == true) {
      Run_Control = false;

      bool mot_en = ( SubProt->GetNumClients() != 0)? true: false;
      MotorControl->EnableMotor(mot_en);
      MotorControl->Run();


    }
  }
  return 0;
}

/* ======================== */
/* ======================== */
