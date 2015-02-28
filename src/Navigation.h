/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
//  *******************************************************************************************
#ifndef __NAVIGATION__
#define __NAVIGATION__
//  *******************************************************************************************
#include <string>

#include "RTT_Interface.h"
#include "CmdModule.h"
#include "Logger.h"

//  *******************************************************************************************
typedef struct {
  float x;
  float y;
  float z;
  float yaw;
  float roll;
  float pitch;

} ControlVector;

//  *******************************************************************************************
class NavUpdate_Interface {

public:
  virtual void UpdateControlVector(const ControlVector & vec) = 0;
};

//  *******************************************************************************************

class Navigation: CmdModule, RTT_Interface {

public:
  Navigation(const char *filename);
  void Run_Task(void);

  const string GetConfigData(void);
  void Update(const char *packet, JSON_Object *msg);
  const string GetData(void);

  bool NewVector(void);
  ControlVector GetVector(void);

	void SetUpdateInterface(NavUpdate_Interface *iface) { Interface = iface; }

private:
  ControlVector CVec;
  bool newVec;
  NavUpdate_Interface *Interface;
  time_t Record;
  Logger Log;
};

#endif
