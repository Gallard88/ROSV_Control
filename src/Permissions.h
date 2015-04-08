#ifndef __PERMISSION_H__
#define __PERMISSION_H__
#include <string>

/*	Permission Class
 *	An object to control system wide modules.
 *	I.E. Enable motors when a client connects, disable when it disconnects.
 *	Can be used as boolean logic (On/Off), or as a counter ( On = non-zero value)
 *
 */

class Permission
{
public:

	Permission(std::string name );
	virtual ~Permission();

	bool isEnabled(void);

	void Inc(void);
	void Dec(void);
	void Set(bool en);

private:

	std::string Name;
	unsigned int Value;
	unsigned int Max_Value;
	Permission(const Permission &);
	Permission & operator=(const Permission &);

};

#endif
