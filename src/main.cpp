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
#include "DiveMonitor.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "ControlProtocol.h"
#include "TcpServer.h"
#include "SubControl.h"
#include "SysSettings.h"

/* ======================== */
/**
 * To Add:
 * ControlProtocol
 * TcpServer
 * DataSource
 * CameraManager
 * DiveMonitor
 * SystemSettings
 * DataLogger
 * LightManager
 * SubControl
 *
 */
/* ======================== */
#define RUN_INTERVAL 100000	// 100 ms

/* ======================== */
const char prop_file[] = "./ROSV_Motors.json";
//const char prop_file[] = "/etc/ROSV_Motors.json";
const struct timeval system_time = { 0 , RUN_INTERVAL};
volatile bool Run_Control;

/* ======================== */
TcpServer        *Listner;
ControlProtocol  *Control;
SubControl       *MotorControl;
DiveMonitor      *DiveMon;
LightManager     *LightMan;
CameraManager    *CamMan;
SysSetting       *Settings;

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
  syslog(LOG_EMERG, "System shutting down");
  closelog();
}

/* ======================== */
char *SkipSpace(char *ptr)
{
  while ( *ptr && isspace(*ptr))
    ptr++;
  return ptr;
}

/* ======================== */
char *SkipChars(char *ptr)
{
  while ( *ptr && !isspace(*ptr))
    ptr++;
  return ptr;
}

/* ======================== */
int setPos(int fd, string arg)
{
  char *ptr = (char *)arg.c_str();
  INS_Bearings pos;

  ptr = SkipSpace(ptr);
  pos.x = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  pos.y = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  pos.z = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  pos.roll = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  pos.pitch = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  pos.yaw = ::atof(ptr);

  printf("Pos: %s\n", arg.c_str());
  return MotorControl->SetTargetPos(pos);
}

/* ======================== */
int setVel(int fd, string arg)
{
  char *ptr = (char *)arg.c_str();
  INS_Bearings vel;

  ptr = SkipSpace(ptr);
  vel.x = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  vel.y = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  vel.z = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  vel.roll = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  vel.pitch = ::atof(ptr);
  ptr = SkipChars(ptr);
  ptr = SkipSpace(ptr);
  vel.yaw = ::atof(ptr);

  return MotorControl->SetTargetVel(vel);
}


/* ======================== */
int getRealPos(int fd, string arg)
{
  string msg;
  INS_Bearings pos;

  pos = INS_GetPosition();

  msg = "getRealPos: ";
  msg += pos.x;
  msg += ", ";
  msg += pos.y;
  msg += ", ";
  msg += pos.z;
  msg += ", ";
  msg += pos.roll;
  msg += ", ";
  msg += pos.pitch;
  msg += ", ";
  msg += pos.yaw;
  msg += "\r\n";

  return write(fd, msg.c_str(), msg.size()+1);
}

/* ======================== */
int getRealVel(int fd, string arg)
{
  string msg;
  INS_Bearings pos;

  pos = INS_GetVelocity();

  msg = "getRealPos: ";
  msg += pos.x;
  msg += ", ";
  msg += pos.y;
  msg += ", ";
  msg += pos.z;
  msg += ", ";
  msg += pos.roll;
  msg += ", ";
  msg += pos.pitch;
  msg += ", ";
  msg += pos.yaw;
  msg += "\r\n";

  return write(fd, msg.c_str(), msg.size()+1);
}

/* ======================== */
int getTargetPos(int fd, string arg)
{
  char msg[256];
  INS_Bearings data;

  data = MotorControl->Position;
  sprintf(msg, "getTargetPos: %2.0f, %2.0f, %2.0f, %2.0f, %2.0f, %2.0f\r\n", data.x,data.y, data.z, data.roll, data.pitch, data.yaw);
  return write(fd, msg, strlen(msg));
}

/* ======================== */
int getTargetVel(int fd, string arg)
{
  char msg[256];
  INS_Bearings data;

  data = MotorControl->Velocity;
  sprintf(msg, "getTargetVel: %2.0f, %2.0f, %2.0f, %2.0f, %2.0f, %2.0f\r\n", data.x,data.y, data.z, data.roll, data.pitch, data.yaw);
  return write(fd, msg, strlen(msg));
}

/* ======================== */
int CamStart(int fd, string arg)
{
  CamMan->Start();
  return 0;
}

/* ======================== */
int CamStop(int fd, string arg)
{
  CamMan->Stop();
  return 0;
}

/* ======================== */
int Brightness(int fd, string arg)
{
  return 0;
}

/* ======================== */
int SetControlMode(int fd, string arg)
{
  MotorControl->SetMode(arg);
  return 0;
}

/* ======================== */
int GetControlMode(int fd, string arg)
{
  string msg;

  msg += "getControlMode: ";
  msg += MotorControl->GetMode();
  msg += "\r\n";
  return write(fd, msg.c_str(), msg.size()+1);
}

/* ======================== */
int main (int argc, char *argv[])
{
  JSON_Object *settings;
  JSON_Value *val;
  int rv;

  openlog("ROSV_Control", LOG_PID, LOG_USER);
  syslog(LOG_EMERG, "Starting program");

  Setup_SignalHandler();
  alarm_wakeup(0);
  atexit(System_Shutdown);

  /* --------------------------------------------- */
  // load paramaters
  val = json_parse_file(prop_file);
  rv = json_value_get_type(val);
  if ( rv != JSONObject ) {
    printf("JSON Parse file failed\n");
    syslog(LOG_EMERG, "JSON Parse file failed\n");
    return -1;
  }

  settings = json_value_get_object(val);
  if ( settings == NULL ) {
    printf("Settings == NULL\n");
    return -1;
  }

  /* --------------------------------------------- */
  if ( PWM_Connect() < 0 ) {
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
//		Settings = new SysSetting("/var/log/");
    cout << "Operating as Root" << endl;
  } else {
//		Settings = new SysSetting("./");
    cout << "Operating as User " << uid << endl;
  }
  Settings = new SysSetting("./");

  /* --------------------------------------------- */
  // open sub-modules

  Listner = new TcpServer(8090);
  MotorControl = new SubControl(settings);

  DiveMon = new DiveMonitor(settings);
  LightMan = new LightManager(settings);
  CamMan = new CameraManager(settings);

  Control = new ControlProtocol();
  Control->AddDataSource(Listner);

  /* --------------------------------------------- */
  // register call backs
  /** =============================== **/
  Control->AddCallback("setPos", setPos);
  Control->AddCallback("setVel", setVel);
  Control->AddCallback("getRealPos", getRealPos);
  Control->AddCallback("getRealVel", getRealVel);
  Control->AddCallback("getTargetPos", getTargetPos);
  Control->AddCallback("getTargetVel", getTargetVel);

  /** =============================== **/
  Control->AddCallback("setControlMode", SetControlMode);
  Control->AddCallback("getControlMode", GetControlMode);

  /** =============================== **/
  Control->AddCallback("CamStart", CamStart);
  Control->AddCallback("CamStop", CamStart);
  Control->AddCallback("Bright", Brightness);

  /* --------------------------------------------- */
  cout << "Starting Main Program" << endl;
  while (1) {
    // read data from connected clients.
    Control->Run(&system_time);

    if ( Run_Control == true) {
      Run_Control = false;
      MotorControl->Run();
      DiveMon->Run();
    }
  }
  return 0;
}

/* ======================== */
/* ======================== */
