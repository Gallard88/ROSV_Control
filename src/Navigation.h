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
#include <string>

#include "RTT_Interface.h"
#include "CmdModule.h"
#include "SubControl.h"

//  *******************************************************************************************
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

private:
  ControlVector CVec;
  bool newVec;
};

