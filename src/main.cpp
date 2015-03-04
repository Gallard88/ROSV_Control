/* ======================== */
#include <signal.h>
#include <cstring>
#include <syslog.h>
#include <unistd.h>
#include <PWM_Controller.h>

#include <RealTimeTask.h>
#include <RTT_Interface.h>
#include <RT_TaskManager.h>

#include "PowerManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Navigation.h"
#include "Motor.h"
#include "SubControl.h"
#include "SubProtocol.h"
#include "Logger.h"

using namespace std;

/* ======================== */
/* ======================== */

/* ======================== */
/* ======================== */
PWM_Con_t PwmModule;
SubControl       *MotorControl;
SubProtocol      *SubProt;
LightManager     *LightMan;
CameraManager    *CamMan;
PowerManager     *Power;
Logger            Log;
Navigation       *Nav;
RT_TaskManager    TaskMan;

//static thread ListenThread;
volatile bool RunSystem;

/* ======================== */
class MainCallBack: public RT_TaskMan_Interface {
public:
  void Deadline_Missed(const std::string & name) {
    syslog(LOG_NOTICE, "Task %s: Deadline missed\n", name.c_str());
    printf("Task %s: Deadline missed\n", name.c_str());
  }
  void Deadline_Recovered(const std::string & name) {
    syslog(LOG_NOTICE, "Task %s: Deadline recovered\n", name.c_str());
    printf("Task %s: Deadline recovered\n", name.c_str());
  }
  void Duration_Overrun(const std::string & name) {
    syslog(LOG_NOTICE, "Task %s: Duration overrun\n", name.c_str());
    printf("Task %s: Duration overrun\n", name.c_str());
  }
};

/* ======================== */
class Main_SubListen: SubProt_Interface {
public:
  Main_SubListen(): NumClients(0) {
  };
  void Client_Added(const string & name, int handle) {
    NumClients++;
    MotorControl->EnableMotor(true);
    LightMan->Enable(true);
    syslog(LOG_NOTICE, "Client %s added, handle = %d", name.c_str(), handle);
  }
  void Client_Removed(int handle) {
    NumClients--;
    if ( NumClients == 0 ) {
      LightMan->Enable(false);
      MotorControl->EnableMotor(false);
    }
    syslog(LOG_NOTICE, "Client removed, handle = %d", handle);
  }

  int NumClients;
};

/* ======================== */
MainCallBack TaskCallback;
Main_SubListen SubListener;

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

  Log.RecordValue("ROSV_Control", "Shutdown", 1);
  syslog(LOG_NOTICE, "System shutting down");
  closelog();
}

/* ======================== */
static void Init_Modules(void)
{
  RealTimeTask *task;

  TaskMan.AddCallback((RT_TaskMan_Interface *)&TaskCallback);

  SubProt = new SubProtocol();
  SubProt->AddListener((SubProt_Interface *) &SubListener);

  MotorControl = new SubControl("/etc/ROSV_Control/motors.json");
  SubProt->AddModule("Motor",      (CmdModule *) MotorControl );
  MotorControl->EnableMotor(false);
  task = new RealTimeTask("Motor", (RTT_Interface *) MotorControl);
  task->SetFrequency(10);
  task->SetMaxDuration(50);
  TaskMan.AddTask(task);

  Nav = new Navigation("/etc/ROSV_Control/navigation.json");
  SubProt->AddModule("Navigation", (CmdModule *) Nav );
  Nav->SetUpdateInterface((NavUpdate_Interface *) MotorControl);
  task = new RealTimeTask("Navigation", (RTT_Interface *) Nav);
  task->SetFrequency(10);
  task->SetMaxDuration(50);
  TaskMan.AddTask(task);

  LightMan = new LightManager("/etc/ROSV_Control/lighting.json");
  LightMan->Pwm = PwmModule;
  SubProt->AddModule("Light",      (CmdModule *) LightMan );
  task = new RealTimeTask("Light", (RTT_Interface *) LightMan);
  task->SetFrequency(1);
  task->SetMaxDuration(50);
  TaskMan.AddTask(task);

  Power = new PowerManager("/etc/ROSV_Control/power.json", PwmModule);
  SubProt->AddModule("Power",      (CmdModule *) Power );
  task = new RealTimeTask("Power", (RTT_Interface *) Power);
  task->SetFrequency(1);
  task->SetMaxDuration(100);
  TaskMan.AddTask(task);

  CamMan = new CameraManager("/etc/ROSV_Control/camera.json");
  SubProt->AddModule("Camera",     (CmdModule *) CamMan );
  task = new RealTimeTask("Camera", (RTT_Interface *) CamMan);
  task->SetFrequency(1);
  task->SetMaxDuration(50);
  TaskMan.AddTask(task);

}

/* ======================== */
int main (int argc, char *argv[])
{
  openlog("ROSV_Control", LOG_PID, LOG_USER);
  syslog(LOG_NOTICE, "Starting program");

  bool daemonise = false;
  int opt;

  while ((opt = getopt(argc, argv, "dD:")) != -1) {
    switch(opt) {
    case 'd':
      daemonise = true;
      break;
    }
  }

  if ( daemonise == true ) {
    syslog(LOG_ALERT, "Daemonising");
    int rv = daemon( 0, 0 );
    if ( rv < 0 ) {
      syslog(LOG_EMERG, "Daemonise failed" );
      exit(-1);
    }
  }

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
  Motor_Set(PwmModule);
  Motor::SetRamp(1.0);

  /* --------------------------------------------- */
  // open sub-modules
  Log.RecordValue("ROSV_Control","Start", 1);

  RunSystem = true;
  Init_Modules();

  syslog(LOG_ALERT, "Main()");
  /* --------------------------------------------- */
  while ( RunSystem ) {

    long time = TaskMan.RunTasks();

    // read data from connected clients.
    if ( time > 0 ) {
      struct timeval system_time;
      system_time.tv_sec = 0;
      system_time.tv_usec =  10000;
      SubProt->Run(system_time);
    }
  }
  return 0;
}

/* ======================== */
/* ======================== */
