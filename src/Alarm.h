#ifndef __ALARM_H__
#define __ALARM_H__

#include <string>
#include <vector>

/*
 *	Alarm
 *	This is a class to track errors and problems.
 *	Active Alarm (not CLEAR) may be muted. Muted alarm returns CLEAR.
 *	Muted status is cleared upon state change.
 *	Alarm may be latched, I.E. can increase in severity, but not decrease.
 */
enum Severity {
  CLEAR,
  WARNING,
  ERROR
};


class Alarm
{
public:
	Alarm(std::string name_, bool mutable_, bool latching_);
	virtual ~Alarm();

	enum Severity GetState(void) const;
	std::string GetName(void) const;

	// These functions can set the state of the object
	void SetState(enum Severity state);
	void SetClear(void);
	void SetWarning(void);
	void SetError(void);

	// Mutes alarm. Only mutes if the alarm is not clear, and CAN be muted.
	// Muted status is cleared when alarm changes state.
	void SetMuted(void);

	static const char *GetSeverityName(enum Severity s);

private:

	std::string Name;
	enum Severity State;
	bool Muteable;
	bool Latching;

	bool Muted;
	Alarm(const Alarm&);
	Alarm & operator=(const Alarm&);
};

/* ========================================== */
/*	Alarm Group Class.
 *
 *	A class to group alarms. This allows a system to combine many alarms into one.
 * 	This makes it easier to hide complex logic, e.g. multiple Temperature alarms can be merged into 
 *	a single actionable item, while each retaining individual reporting.
 */

class AlarmGroup
{
public:
	AlarmGroup(std::string name);
	~AlarmGroup();

	// Add an alarm to a group.
	void AddAlarm(const Alarm * alm);

	//	This functions scans the group of errors and reports the highest severity found.
	enum Severity GetGroupState(void);

private:
	std::vector<const Alarm *> AlarmList;
	std::string Name;
	enum Severity GroupState;

	AlarmGroup(const AlarmGroup&);
	AlarmGroup & operator=(const AlarmGroup&);

};


#endif
