//  *******************************************************************************************
//  *******************************************************************************************

using namespace std;

#include <cstring>
#include "Navigation.h"

//  *******************************************************************************************
//  *******************************************************************************************
Navigation::Navigation(const char *filename):
  newVec(true)
{
  // in time we will use this to parse a json file for system limits.
  memset(&CVec, 0, sizeof(ControlVector));
}

//  *******************************************************************************************
void Navigation::Run_Task(void)
{
  time_t t = time(NULL);
  if ( Record != t ) {
    Record = t;
    Log->RecordValue("Nav", "Forward", CVec.x);
    Log->RecordValue("Nav", "Strafe",  CVec.y);
    Log->RecordValue("Nav", "Dive",    CVec.z);
    Log->RecordValue("Nav", "Turn",    CVec.yaw);
  }
}

//  *******************************************************************************************
void Navigation::Update(const char *packet, JSON_Object *msg)
{
  if ( strcmp("SetVector", packet) == 0 ) {
    PacketTime = time(NULL);
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
    if ( Interface != NULL ) {
      Interface->UpdateControlVector(CVec);
    }
    newVec = true;
  }
}

//  *******************************************************************************************
const string Navigation::GetData(void)
{
  string msg;
  char vec[256];

  msg = "\"RecordType\": \"ReportVectors\", ";
  msg += "\"Vectors\":[";
  sprintf(vec, "{ \"Ch\":\"Forward\", \"Mode\":\"Raw\", \"Target\": %2.2f }, ", CVec.x );
  msg += string(vec);
  sprintf(vec, "{ \"Ch\":\"Strafe\", \"Mode\":\"Raw\", \"Target\": %2.2f }, ", CVec.y );
  msg += string(vec);
  sprintf(vec, "{ \"Ch\":\"Dive\", \"Mode\":\"Raw\", \"Target\": %2.2f }, ", CVec.z );
  msg += string(vec);
  sprintf(vec, "{ \"Ch\":\"Turn\", \"Mode\":\"Raw\", \"Target\": %2.2f }", CVec.yaw );
  msg += string(vec);

  msg += " ]";
  return msg;
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
