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

#include "CmdModule.h"
#include "SubControl.h"

//  *******************************************************************************************
//  *******************************************************************************************

class Navigation: CmdModule {

public:
  Navigation(const char *filename);
  void Run(void);

  const string GetConfigData(void);
  void Update(const char *packet, JSON_Object *msg);
  const string GetData(void);

  bool NewVector(void);
  ControlVector GetVector(void);

private:
  ControlVector CVec;
  bool newVec;
};

