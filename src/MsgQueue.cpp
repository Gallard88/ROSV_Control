
#include <parson.h>

#include "MsgQueue.h"

using namespace std;

MsgQueue::MsgQueue(std::string module, bool update):
  Module(module), Update(update), Broadcast(false)
{
  RxMsgs.reserve(100);
}

void MsgQueue::SetBroadcast(void)
{
  Broadcast = true;
}

bool MsgQueue::MsgWaiting(void)
{
  return (TxMsgs.size() != 0)? true: false;
}

std::string MsgQueue::GetMsg(void)
{
  string s;
  if ( TxMsgs.size() != 0 ) {
    s = TxMsgs[0];
    TxMsgs.erase(TxMsgs.begin());
  }
  return s;
}

void MsgQueue::Store(const std::string & msg)
{
  if ( Update == false ) {
    return;
  }

  JSON_Value *data = json_parse_string(msg.c_str());
  if ( data == NULL ) {
    return;
  }
  JSON_Object *obj = json_value_get_object(data);

  const char *module = json_object_get_string(obj, "Module");
  if ( module != NULL ) {
    if ( Module.compare(module) == 0 ) {
      RxMsgs.push_back(msg);
    }
  }
  // release the data now that we are finished.
  json_value_free(data);
}

bool MsgQueue::ReceiveReady(void)
{
  return (RxMsgs.size () != 0)? true : false;
}

void MsgQueue::Send(const std::string & record, const std::string & data)
{
  string json_msg;
  json_msg += "{ \"Module\":\"" + Module + "\",";
  json_msg += "\"RecordType\":\"" + record + "\", ";
  json_msg += "\"Data\": { " + data + "}}\r\n";
  TxMsgs.push_back(json_msg);
}

bool MsgQueue::Receive(std::string * type, std::string *data)
{
  if ( RxMsgs.size() == 0 ) {
    return false;
  }
  bool rv = false;
  string s = RxMsgs[0];
  RxMsgs.erase(RxMsgs.begin());

  JSON_Value *p = json_parse_string(s.c_str());

  if ( p == NULL ) {
    return false;
  }
  JSON_Object *obj = json_value_get_object(p);

  const char *packet = json_object_get_string(obj, "Packet");
  if ( packet != NULL ) {
    *type = string(packet);
    *data = s;
    rv = true;
  }
  json_value_free(p);
//  cout << "Size: " << RxMsgs.size() << endl;
  return rv;
}

bool MsgQueue::IsBroadcast(void)
{
  bool rv = Broadcast;
  Broadcast = false;
  return rv;
}
