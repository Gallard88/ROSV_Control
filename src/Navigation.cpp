//  *******************************************************************************************
//  *******************************************************************************************
#include <cstring>
#include <iostream>

#include "Navigation.h"
#include "MsgQueue.h"

using namespace std;

//  *******************************************************************************************
//  *******************************************************************************************
Navigation::Navigation(SubControl * motors):
  Motors(motors)
{
  CVec = { 0 };
  Log[0].SetName("Nav", "Forward");
  Log[1].SetName("Nav", "Strafe");
  Log[2].SetName("Nav", "Dive");
  Log[3].SetName("Nav", "Turn");
  MQue = new MsgQueue("Navigation", true);
}

MsgQueue *Navigation::GetQueue(void)
{
  return MQue;
}

//  *******************************************************************************************
void Navigation::Run_Task(void)
{
  float update_vector[SubControl::vecSize];

  while ( MQue->ReceiveReady() == true ) {
    string type, line;
    if ( MQue->Receive(&type, &line) == true ) {
      Parse(line.c_str());
    }
  }

  update_vector[SubControl::vecX] = CVec.x;
  update_vector[SubControl::vecY] = CVec.y;
  update_vector[SubControl::vecZ] = CVec.z;
  update_vector[SubControl::vecROLL]  = CVec.roll;
  update_vector[SubControl::vecPITCH] = CVec.pitch;
  update_vector[SubControl::vecYAW]   = CVec.yaw;

  Motors->Update(update_vector);
}

//  *******************************************************************************************
void Navigation::Update(const char *packet, JSON_Object *msg)
{
}

void Navigation::Parse(const char *msg)
{
  JSON_Value *data = json_parse_string(msg);
  if ( data == NULL ) {
    cout << "Null Ptr" << endl;
    return;
  }
  JSON_Object *obj = json_value_get_object(data);

  const char *packet = json_object_get_string(obj, "Packet");
  if ( strcmp("SetVector", packet) != 0 ) {
    cout << "Wierd Packet: " << msg << endl;
    json_value_free(data);
    return;
  }

  const char *mode = json_object_get_string(obj, "Mode");
  if ( strcmp("Raw", mode) != 0 ) {
    // unknown mode, ignore this packet for the moment.
    cout << "Not Raw: " << msg << endl;
    json_value_free(data);
    return;
  }
  const char *ch = json_object_get_string(obj, "Ch");
  float value = json_object_get_number(obj, "Value");

  if ( strcmp("Forward", ch) == 0 ) {
    CVec.x = value;
    Log[0].Set(value);
  } else if ( strcmp("Strafe", ch) == 0 ) {
    CVec.y = value;
    Log[1].Set(value);
  } else if ( strcmp("Dive", ch) == 0 ) {
    CVec.z = value;
    Log[2].Set(value);
  } else if ( strcmp("Turn", ch) == 0 ) {
    CVec.yaw = value;
    Log[3].Set(value);
  }
  FlagReady();
  json_value_free(data);
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
ControlVector Navigation::GetVector(void)
{
  return CVec;
}

//  *******************************************************************************************
//  *******************************************************************************************
