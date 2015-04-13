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
#include "AlarmManager.h"

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
AlarmManager     *AlmManager;

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
MainCallBack TaskCallback;

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
  delete AlmManager;

  Log.RecordValue("ROSV_Control", "Shutdown", 1);
  syslog(LOG_NOTICE, "System shutting down");
  closelog();
}

/* ======================== */
static void Init_Modules(void)
{
  RealTimeTask *task;

  TaskMan.AddCallback((RT_TaskMan_Interface *)&TaskCallback);

  syslog(LOG_NOTICE, "Init_Modules()");
  SubProt = new SubProtocol();

  syslog(LOG_NOTICE, "Motors()");
  MotorControl = new SubControl("/etc/ROSV_Control/motors.json");
  SubProt->AddModule("Motor",      (CmdModule *) MotorControl );
  task = new RealTimeTask("Motor", (RTT_Interface *) MotorControl);
  task->SetFrequency(10);
  task->SetMaxDuration(50);
  TaskMan.AddTask(task);

  syslog(LOG_NOTICE, "Nav()");
  Nav = new Navigation("/etc/ROSV_Control/navigation.json");
  SubProt->AddModule("Navigation", (CmdModule *) Nav );
  Nav->SetUpdateInterface((NavUpdate_Interface *) MotorControl);
  task = new RealTimeTask("Navigation", (RTT_Interface *) Nav);
  task->SetFrequency(10);
  task->SetMaxDuration(50);
  TaskMan.AddTask(task);

  syslog(LOG_NOTICE, "Lighting()");
  LightMan = new LightManager("/etc/ROSV_Control/lighting.json");
  LightMan->Pwm = PwmModule;
  SubProt->AddModule("Light",      (CmdModule *) LightMan );
  task = new RealTimeTask("Light", (RTT_Interface *) LightMan);
  task->SetFrequency(1);
  task->SetMaxDuration(50);
  TaskMan.AddTask(task);

  syslog(LOG_NOTICE, "Power()");
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

  AlmManager = new AlarmManager();
  SubProt->AddModule("AlarmManager", (CmdModule *) AlmManager );


  // Wire up alarms
  MotorControl->Add(Power->getVoltAlarmGroup());
  MotorControl->Add(Power->getTempAlarmGroup());

  AlmManager->add(Power->getVoltAlarmGroup());
  AlmManager->add(Power->getTempAlarmGroup());

  LightMan->Add(Power->getVoltAlarmGroup());
  LightMan->Add(Power->getTempAlarmGroup());

  MotorControl->Add(SubProt->getPermGroup());
  LightMan->Add(SubProt->getPermGroup());
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
