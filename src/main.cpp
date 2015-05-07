/* ======================== */
#include <signal.h>
#include <cstring>
#include <unistd.h>
#include <PWM_Controller.h>

#include <RealTimeTask.h>
#include <RTT_Interface.h>
#include <RT_TaskManager.h>

#include <thread>

#include "config.h"

#include "PowerManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Navigation.h"
#include "SubControl.h"
#include "SubProtocol.h"
#include "AlarmManager.h"
#include "PermissionManager.h"
#include "EventMessages.h"
#include "VideoStream.h"

using namespace std;
using namespace RealTime;

/* ======================== */
/* ======================== */
PWM_Con_t PwmModule;
SubControl       *MotorControl;
SubProtocol      *SubProt;
LightManager     *LightMan;
CameraManager    *CamMan;
PowerManager     *Power;
Navigation       *Nav;
RT_TaskManager   *TaskMan;
AlarmManager     *AlmManager;
PermGroupManager *PManager;
EventMsg         *Msg;
VideoStreamer    *Video;

volatile bool RunSystem;

static void RunVideo(void)
{
  Msg->Log(EventMsg::NOTICE, "RunVideo()");
  while ( RunSystem ) {
    Video->RunStream();
  }
}

/* ======================== */
class MainCallBack: public RealTime::Reporting_Interface {
public:
  void Deadline_Missed(const std::string & name) {
    Msg->Log(EventMsg::NOTICE, "Task %s: Deadline missed", name.c_str());
  }
  void Deadline_Recovered(const std::string & name) {
    Msg->Log(EventMsg::NOTICE,"Task %s: Deadline recovered", name.c_str());
  }
  void Duration_Overrun(const std::string & name) {
    Msg->Log(EventMsg::NOTICE, "Task %s: Duration overrun", name.c_str());
  }
  void Statistics(const std::string & name, RealTimeTask::Statistics_t stats) {
    Msg->Log(EventMsg::NOTICE, "========================" );
    Msg->Log(EventMsg::NOTICE, "Task %s", name.c_str() );
    Msg->Log(EventMsg::NOTICE, "   Min  = %u", stats.Min );
    if ( stats.Max >= 1000000 ) {
      Msg->Log(EventMsg::NOTICE, "   Max  = %u.%06u S", stats.Max / 1000000, stats.Max % 1000000 );
    } else {
      Msg->Log(EventMsg::NOTICE, "   Max  = %u uS", stats.Max );
    }
//    Msg->Log(EventMsg::NOTICE, "   Avg  = %u", stats.Avg );
    Msg->Log(EventMsg::NOTICE, "   Call = %u", stats.Called );
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
  delete TaskMan;
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
  delete Video;

  Msg->Log(EventMsg::NOTICE, "System Shutdown");
}

/* ======================== */
static void Init_Modules(void)
{
  RealTimeTask *task;

  // connect to other external systems
  PwmModule = PWM_Connect();
  if ( PwmModule == NULL ) {
    Msg->Log(EventMsg::ERROR, "PWM_Connect() failed");
    exit(-1);
  }

  Video = new VideoStreamer(8091);

  TaskMan = new RT_TaskManager();
  TaskMan->AddCallback((Reporting_Interface *)&TaskCallback);

  SubProt = new SubProtocol();

  SubProt->Add(Msg->GetQueue());

  MotorControl = new SubControl("/etc/ROSV_Control/motors.json", PwmModule);
  SubProt->Add(MotorControl->GetQueue());

  Nav = new Navigation(MotorControl);
  SubProt->Add(Nav->GetQueue());
  task = new RealTimeTask("Navigation", (Task_Interface *) Nav);
  task->SetFrequency(10);
  task->SetMaxDuration_Ms(50);
  TaskMan->AddTask(task);

  LightMan = new LightManager("/etc/ROSV_Control/lighting.json");
  LightMan->Pwm = PwmModule;
  SubProt->Add(LightMan->GetQueue());
  task = new RealTimeTask("Light", (Task_Interface *) LightMan);
  task->SetFrequency(1);
  task->SetMaxDuration_Ms(50);
  TaskMan->AddTask(task);

  Power = new PowerManager("/etc/ROSV_Control/power.json", PwmModule);
  SubProt->Add(Power->GetQueue());
  task = new RealTimeTask("Power", (Task_Interface *) Power);
  task->SetFrequency(1);
  task->SetMaxDuration_Ms(100);
  TaskMan->AddTask(task);

  CamMan = new CameraManager("/etc/ROSV_Control/camera.json");
  SubProt->Add(CamMan->GetQueue());
  task = new RealTimeTask("Camera", (Task_Interface *) CamMan);
  task->SetFrequency(1);
  task->SetMaxDuration_Ms(50);
  TaskMan->AddTask(task);

  AlmManager = new AlarmManager();
  SubProt->Add(AlmManager->GetQueue());

  PManager = new PermGroupManager();
  SubProt->Add(PManager->GetQueue());

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

static void PrintVersionInfo(void)
{
  char name[256];
  gethostname(name, sizeof(name));
  Msg->Log(EventMsg::NOTICE, PACKAGE_STRING);
  Msg->Log(EventMsg::NOTICE, PACKAGE_BUGREPORT);
  Msg->Log(EventMsg::NOTICE, name);
}


/* ======================== */
int main (int argc, char *argv[])
{

  Msg = EventMsg::Init();
  Msg->setFilename("/var/log/ROSV/ROSV_Log");
  Msg->setLogDepth(5);
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
  Msg->Log(EventMsg::NOTICE, "Starting Program");
  PrintVersionInfo();

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
  // open sub-modules
  Msg->Log(EventMsg::NOTICE, "Starting System");
  RunSystem = true;
  Init_Modules();

  std::thread VideoThread (RunVideo);

  Msg->Log(EventMsg::NOTICE, "Main()");
  /* --------------------------------------------- */
  while ( RunSystem ) {

    long time = TaskMan->RunTasks();

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
