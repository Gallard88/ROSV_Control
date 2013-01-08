
#ifndef __MOTOR_H__
#define __MOTOR_H__
/* ======================== */

#include <string>
#include "parson.h"

class Motor
{
private:
	double Scale_Factor[3];
	// forward, side, turn.
	// 0, + = forward
	// 1, + = right
	// 2, + = clockwise turn.
	double MaxPower;


public:
	string Name;	// motor name, doubles as a command for now.

	Motor(JSON_Object *obj, string name);
	double GetPower(double *direction);

};

/* ======================== */
#endif
