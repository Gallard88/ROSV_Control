
#include <syslog.h>

#include "Alarm.h"

using namespace std;

const char *SeverityNames[3] = {
  "Clear",
  "Warning",
  "Error"
};

Alarm::Alarm(string name_, bool mutable_, bool latching_):
  Name(name_), State(CLEAR),
  Muteable(mutable_), Latching(latching_),
  Muted(false)
{
}

Alarm::~Alarm()
{
  Name.clear();
}

const char *GetSeverityName(enum Severity s)
{
  return SeverityNames[s];
}

enum Severity Alarm::GetState(void) const
{
  if (( State == WARNING ) && ( Muteable == true )) {
    return CLEAR;
  }
  return State;
}

string Alarm::GetName(void) const
{
  return Name;
}

void Alarm::SetState(enum Severity state)
{
  if ( this->State == state ) {
    return;	// this means we only get below on an edge.
  }
  switch ( this-> State ) {
  case CLEAR:
    this->State = state;
    break;

  case WARNING:
    // Severity can be CLEAR or ERROR.
    // We can always increase Severity,
    // but need to check is we can decrease it.
    if (( state == ERROR ) ||
        ( this->Latching == false )) {
      this->State = state;
    }
    break;

    // Need to check if we can go back (non-latching)
  case ERROR:
    if ( this->Latching == false ) {
      this->State = state;
    }
    break;
  }
  // When an alarm changes state we also clear that it may have been muted.
  // That way the changes in severity is reported.
  this->Muted = false;

  // Record state change.
  syslog(LOG_NOTICE, "Alarm %s => %s", Name.c_str(), SeverityNames[State]);
}

void Alarm::SetClear(void)
{
  SetState(CLEAR);
}

void Alarm::SetWarning(void)
{
  SetState(WARNING);
}

void Alarm::SetError(void)
{
  SetState(ERROR);
}

void Alarm::SetMuted(void)
{
  if (( this->Muteable == true ) &&
      ( this->Muted == false ) &&
      ( this->State != CLEAR )) {
    // We can mute alarms, alarm IS NOT muted,
    // and alarm is NOT clear
    this->Muted = true;
    // record that alarm has been muted.
    syslog(LOG_NOTICE, "Alarm %s (%s) muted", Name.c_str(), SeverityNames[State]);
  }
}

/* ========================================== */
AlarmGroup::AlarmGroup(std::string name):
  Name(name), GroupState(CLEAR)
{
}

AlarmGroup::~AlarmGroup()
{
  Name.clear();
  AlarmList.clear();
}

void AlarmGroup::AddAlarm(const Alarm * alm)
{
  AlarmList.push_back(alm);
  syslog(LOG_NOTICE, "Alarm %s added to Group %s ", alm->GetName().c_str(), Name.c_str());
}

enum Severity AlarmGroup::GetGroupState(void)
{
  enum Severity global = CLEAR;

  for ( size_t i = 0; i < AlarmList.size(); i ++ ) {

    enum Severity local = AlarmList[i]->GetState();

    //	We can only move forward in severity.
    //	Function returns the highest Severity found.
    switch ( global ) {
    case CLEAR:
      global = local;
      break;
    case WARNING:
      if ( local == ERROR ) {
        global = ERROR;
      }
      break;
    case ERROR:
      break;
    }
  }

  if ( GroupState != global ) {
    syslog(LOG_NOTICE, "Alarm Group %s => %s", Name.c_str(), SeverityNames[global]);
    GroupState = global;
  }
  return global;
}
