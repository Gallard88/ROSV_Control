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

#include "PowerManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Navigation.h"
#include "SubControl.h"
#include "SubProtocol.h"
#include "Logger.h"

/* ======================== */
/* ======================== */
#define RUN_INTERVAL  (10*1000)    // us

/* ======================== */
const struct timeval system_time = { 0 , RUN_INTERVAL};

/* ======================== */
PWM_Con_t PwmModule;
SubControl       *MotorControl;
SubProtocol      *SubProt;
LightManager     *LightMan;
CameraManager    *CamMan;
PowerManager     *Power;
Logger           *Log;
Navigation       *Nav;

//static thread ListenThread;
volatile bool RunSystem;

/* ======================== */
/* ======================== */
void SignalHandler_Setup(void)
{
  struct sigaction sig;

  // Install timer_handler as the signal handler for SIGVTALRM.
  memset (&sig, 0, sizeof (struct sigaction));
  sig.sa_handler = &exit;
  sigaction (SIGINT , &sig, NULL);
  sigaction (SIGTERM , &sig, NULL);
  sigaction (SIGABRT , &sig, NULL);
}

/* ======================== */
void System_Shutdown(void)
{
  RunSystem = false;
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
int main (int argc, char *argv[])
{
  openlog("ROSV_Control", LOG_PID, LOG_USER);
  syslog(LOG_NOTICE, "Starting program");
  /*
    int rv = daemon( 0, 0 );
    if ( rv < 0 ) {
      syslog(LOG_EMERG, "Daemonise failed" );
      exit(-1);
    }
  */
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

  RunSystem = true;

  MotorControl = new SubControl("/etc/ROSV_Control/motors.json", PwmModule);

  LightMan = new LightManager("/etc/ROSV_Control/lighting.json");
  LightMan->Pwm = PwmModule;

  Power = new PowerManager("/etc/ROSV_Control/power.json", PwmModule);

  Nav = new Navigation("/etc/ROSV_Control/navigation.json");

  CamMan = new CameraManager("/etc/ROSV_Control/camera.json");

  SubProt = new SubProtocol();
  SubProt->Pwm = PwmModule;

  SubProt->AddModule("Light",      (CmdModule *) LightMan );
  SubProt->AddModule("Power",      (CmdModule *) Power );
  SubProt->AddModule("Motor",      (CmdModule *) MotorControl );
  SubProt->AddModule("Camera",     (CmdModule *) CamMan );
  SubProt->AddModule("Navigation", (CmdModule *) Nav );

  /* --------------------------------------------- */
  while ( RunSystem ) {
    // read data from connected clients.
    SubProt->Run(system_time);

    Nav->Run();
    if ( Nav->NewVector() == true ) {
      ControlVector vec = Nav->GetVector();
      MotorControl->SetControlVector( &vec );
    }

    bool mot_en = ( SubProt->GetNumClients() != 0)? true: false;
    MotorControl->EnableMotor(mot_en);
    MotorControl->Run();

    LightMan->Run();
    Power->Run();
  }
  return 0;
}

/* ======================== */
/* ======================== */
