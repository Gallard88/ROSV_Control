//  *******************************************************************************************
#ifndef __NAVIGATION__
#define __NAVIGATION__
//  *******************************************************************************************
#include <RTT_Interface.h>
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
class MsgQueue;

class Navigation: RealTime::Task_Interface {

public:
  Navigation(SubControl * motors);
  void Run_Task(void);

  MsgQueue *GetQueue(void);

  bool NewVector(void);
  ControlVector GetVector(void);

private:
  ControlVector CVec;
  SubControl    *Motors;
  MsgQueue      *MQue;
  Variable Log[4];

  void Parse(const char *msg);
  void SendData(void);
};

#endif
