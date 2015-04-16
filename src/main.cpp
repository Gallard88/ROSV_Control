/* ======================== */
#include <signal.h>
#include <cstring>
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
#include "AlarmManager.h"
#include "PermissionManager.h"
#include "EventMessages.h"

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
Navigation       *Nav;
RT_TaskManager    TaskMan;
AlarmManager     *AlmManager;
PermGroupManager *PManager;
EventMsg         *Msg;

//static thread ListenThread;
volatile bool RunSystem;

/* ======================== */
class MainCallBack: public RT_TaskMan_Interface {
public:
  void Deadline_Missed(const std::string & name) {
    Msg->Log(EventMsg::NOTICE, "Task %s: Deadline missed", name.c_str());
  }
  void Deadline_Recovered(const std::string & name) {
    Msg->Log(EventMsg::NOTICE,"Task %s: Deadline recovered\n", name.c_str());
  }
  void Duration_Overrun(const std::string & name) {
    Msg->Log(EventMsg::NOTICE, "Task %s: Duration overrun\n", name.c_str());
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
  delete PManager;

  Msg->Log(EventMsg::NOTICE, "System Shutdown");
}

/* ======================== */
static void Init_Modules(void)
{
  RealTimeTask *task;

  TaskMan.AddCallback((RT_TaskMan_Interface *)&TaskCallback);

  SubProt = new SubProtocol();

  MotorControl = new SubControl("/etc/ROSV_Control/motors.json");
  SubProt->AddModule("Motor",      (CmdModule *) MotorControl );
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

  AlmManager = new AlarmManager();
  SubProt->AddModule("AlarmManager", (CmdModule *) AlmManager );

  PManager = new PermGroupManager();
  SubProt->AddModule("PermissionManager", (CmdModule *) PManager );

  SubProt->AddModule("Messages", (CmdModule *) EventMsg::Init() );

  // Wire up alarms
  MotorControl->Add(Power->getVoltAlarmGroup());
  MotorControl->Add(Power->getTempAlarmGroup());

  AlmManager->add(Power->getVoltAlarmGroup());
  AlmManager->add(Power->getTempAlarmGroup());

  LightMan->Add(Power->getVoltAlarmGroup());
  LightMan->Add(Power->getTempAlarmGroup());

  MotorControl->Add(SubProt->getPermGroup());
  LightMan->Add(SubProt->getPermGroup());
  PManager->add(SubProt->getPermGroup());
}

/* ======================== */
int main (int argc, char *argv[])
{

  Msg = EventMsg::Init();
  Msg->sendToSyslog("ROSV_Control");
  Msg->setFilename("/home/pi/ROSV_Log.txt");
  Msg->setLogDepth(20);
  Msg->Log(EventMsg::NOTICE, "Starting Program");


  bool daemonise = false;
  int opt;

  while ((opt = getopt(argc, argv, "dD:")) != -1) {
    switch(opt) {
    case 'd':
      daemonise = true;
      break;
    }
  }

  Msg->sendToTerminal(!daemonise);

  if ( daemonise == true ) {
    Msg->Log(EventMsg::NOTICE, "Daemonising");
    int rv = daemon( 0, 0 );
    if ( rv < 0 ) {
      Msg->Log(EventMsg::NOTICE, "Daemonising - failed");
      exit(-1);
    }
  }

  SignalHandler_Setup();
  atexit(System_Shutdown);

  /* --------------------------------------------- */
  // connect to other external systems
  PwmModule = PWM_Connect();
  if ( PwmModule == NULL ) {
    Msg->Log(EventMsg::ERROR, "PWM_Connect() failed");
    return -1;
  }
  Motor_Set(PwmModule);
  Motor::SetRamp(1.0);

  /* --------------------------------------------- */
  // open sub-modules
  Msg->Log(EventMsg::NOTICE, "Starting System");

  RunSystem = true;
  Init_Modules();

  Msg->Log(EventMsg::NOTICE, "Main()");
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
    PManager->Check();
    AlmManager->Check();
  }
  return 0;
}

/* ======================== */
/* ======================== */
