//  *******************************************************************************************
//  *******************************************************************************************

using namespace std;

#include "Navigation.h"
#include <cstring>
#include <iostream>

//  *******************************************************************************************
//  *******************************************************************************************
Navigation::Navigation(const char *filename)
{
  // in time we will use this to parse a json file for system limits.
  memset(&CVec, 0, sizeof(ControlVector));
  newVec = true;
  SetCallPeriod(100);
}

//  *******************************************************************************************
void Navigation::Run(void)
{
  if ( RunModule() == true ) {
    Log->RecordValue("Nav", "Forward", CVec.x);
    Log->RecordValue("Nav", "Strafe",  CVec.y);
    Log->RecordValue("Nav", "Dive",    CVec.z);
    Log->RecordValue("Nav", "Turn",    CVec.yaw);
  }
}

//  *******************************************************************************************
const string Navigation::GetConfigData(void)
{
  return " ";
}

void Navigation::Update(const char *packet, JSON_Object *msg)
{
  if ( strcmp("SetVector", packet) == 0 ) {
    const char *ch = json_object_get_string(msg, "Ch");
    const char *mode = json_object_get_string(msg, "Mode");
    float value = json_object_get_number(msg, "Value");

    if ( strcmp("Raw", mode) != 0 ) {
      // unknown mode, ignore this packet for the moment.
      return;
    }

    if ( strcmp("Forward", ch) == 0 ) {
      CVec.x = value;
    } else if ( strcmp("Strafe", ch) == 0 ) {
      CVec.y = value;
    } else if ( strcmp("Dive", ch) == 0 ) {
      CVec.z = value;
    } else if ( strcmp("Turn", ch) == 0 ) {
      CVec.yaw = value;
    } else {
      return;
    }
    newVec = true;
  }
}

const string Navigation::GetData(void)
{
  return " ";
}

//  *******************************************************************************************
bool Navigation::NewVector(void)
{
  return newVec;
}

ControlVector Navigation::GetVector(void)
{
  newVec = false;
  return CVec;
}

//  *******************************************************************************************
//  *******************************************************************************************
