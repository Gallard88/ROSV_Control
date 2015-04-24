//  *******************************************************************************************
#ifndef __NAVIGATION__
#define __NAVIGATION__
//  *******************************************************************************************
#include <RTT_Interface.h>
#include "CmdModule.h"
#include "Variable.h"
#include "SubControl.h"

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
//  *******************************************************************************************

class Navigation: CmdModule, RealTime::Task_Interface {

public:
  Navigation(SubControl * motors);
  void Run_Task(void);

  void Update(const char *packet, JSON_Object *msg);
  const std::string GetData(void);

  bool NewVector(void);
  ControlVector GetVector(void);

private:
  ControlVector CVec;
  SubControl    *Motors;
  Variable Log[4];
};

#endif
