/* ======================== */
using namespace std;

#include <vector>
#include <signal.h>
#include <cstring>
#include <syslog.h>
#include <PWM_Controller.h>

#include "RealTimeTask.h"
#include "RTT_Interface.h"

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
vector<RealTimeTask *> TaskList;
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
class Main_SubListen: SubProt_Interface {
public:
  Main_SubListen(): NumClients(0) {};
  void Client_Added(const string & name, int handle);
  void Client_Removed(int handle);

  int NumClients;
};

void Main_SubListen::Client_Added(const string & name, int handle)
{
  NumClients++;
  MotorControl->EnableMotor(true);
  syslog(LOG_NOTICE, "Client %s added, handle = %d", name.c_str(), handle);
}
void Main_SubListen::Client_Removed(int handle)
{
  NumClients--;
  if ( NumClients == 0 ) {
    MotorControl->EnableMotor(false);
  }
  syslog(LOG_NOTICE, "Client removed, handle = %d", handle);
}

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

  Log->RecordValue("ROSV_Control", "Shutdown", 1);
  syslog(LOG_NOTICE, "System shutting down");
  closelog();
}

/* ======================== */
static void Init_Modules(void)
{
  RealTimeTask *task;

  SubProt = new SubProtocol();
  SubProt->AddListener((SubProt_Interface *) &SubListener);

  Nav = new Navigation("/etc/ROSV_Control/navigation.json");
  SubProt->AddModule("Navigation", (CmdModule *) Nav );
  task = new RealTimeTask("Navigation", (RTT_Interface *) Nav);
  task->SetFrequency(10);
  TaskList.push_back(task);

  MotorControl = new SubControl("/etc/ROSV_Control/motors.json", PwmModule);
  SubProt->AddModule("Motor",      (CmdModule *) MotorControl );
  MotorControl->EnableMotor(false);
  task = new RealTimeTask("Motor", (RTT_Interface *) MotorControl);
  task->SetFrequency(10);
  TaskList.push_back(task);

  LightMan = new LightManager("/etc/ROSV_Control/lighting.json");
  LightMan->Pwm = PwmModule;
  SubProt->AddModule("Light",      (CmdModule *) LightMan );
  task = new RealTimeTask("Light", (RTT_Interface *) LightMan);
  task->SetFrequency(1);
  TaskList.push_back(task);

  Power = new PowerManager("/etc/ROSV_Control/power.json", PwmModule);
  SubProt->AddModule("Power",      (CmdModule *) Power );
  task = new RealTimeTask("Power", (RTT_Interface *) Power);
  task->SetFrequency(1);
  TaskList.push_back(task);

  CamMan = new CameraManager("/etc/ROSV_Control/camera.json");
  SubProt->AddModule("Camera",     (CmdModule *) CamMan );
  task = new RealTimeTask("Camera", (RTT_Interface *) CamMan);
  task->SetFrequency(1);
  TaskList.push_back(task);
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
  Init_Modules();

  /* --------------------------------------------- */
  while ( RunSystem ) {
    // read data from connected clients.
    SubProt->Run(system_time);

    if ( Nav->NewVector() == true ) {
      ControlVector vec = Nav->GetVector();
      MotorControl->SetControlVector( &vec );
    }

for ( auto& t: TaskList ) {
      t->Run();
      if ( t->DetectDeadlineEdge() ) {
        syslog(LOG_EMERG, "%s Deadline Missed\n", t->GetName().c_str());
      }
    }
  }
  return 0;
}

/* ======================== */
/* ======================== */
