#ifndef __ALARM_H__
#define __ALARM_H__

#include <string>
#include <vector>
#include <memory>

/*
 * Alarm
 * This is a class to track errors and problems.
 * Active Alarm (not CLEAR) may be muted. Muted alarm returns CLEAR.
 * Muted status is cleared upon state change.
 * Alarm may be latched, I.E. can increase in severity, but not decrease.
 */

class Alarm
{
public:
  Alarm(std::string name_, bool mutable_, bool latching_);
  virtual ~Alarm();

  typedef enum {
    CLEAR,
    WARNING,
    ERROR
  } Severity_t;

  Severity_t GetState(void) const;
  std::string GetName(void) const;

  // These functions can set the state of the object
  void SetState(Severity_t state);
  void SetClear(void);
  void SetWarning(void);
  void SetError(void);

  // Mutes alarm. Only mutes if the alarm is not clear, and CAN be muted.
  // Muted status is cleared when alarm changes state.
  void SetMuted(void);

  static const char *GetSeverityName(Severity_t s);

private:

  std::string Name;
  Severity_t State;
  bool Muteable;
  bool Latching;

  bool Muted;

  // disable copying
  Alarm(const Alarm&);
  Alarm & operator=(const Alarm&);
  Alarm & operator+(const Alarm&);
};

/* ========================================== */
/* Alarm Group Class.
 *
 * A class to group alarms. This allows a system to combine many alarms into one.
 * This makes it easier to hide complex logic, e.g. multiple Temperature alarms can be merged into 
 * a single actionable item, while each retaining individual reporting.
 */

class AlarmGroup
{
public:
  AlarmGroup();
  AlarmGroup(std::string name);
  ~AlarmGroup();

  // Add an alarm to a group.
  void AddAlarm(std::shared_ptr<const Alarm > alm);
  void add(const AlarmGroup & alarm);
  void SetName(std::string name);

  //	This functions scans the group of errors and reports the highest severity found.
  Alarm::Severity_t GetGroupState(void);

private:
  std::vector<std::shared_ptr<const Alarm >> AlarmList;
  std::string Name;
  Alarm::Severity_t GroupState;

  // Disable copying
  AlarmGroup(const AlarmGroup&);
  AlarmGroup & operator=(const AlarmGroup&);
  AlarmGroup & operator+(const AlarmGroup&);

};


#endif
