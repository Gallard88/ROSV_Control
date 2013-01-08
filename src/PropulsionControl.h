
#ifndef __PROPULSION_CONTROL_H__
#define __PROPULSION_CONTROL_H__
/* ======================== */

#include <string>
#include <vector>

#include "parson.h"
#include "motor.h"

/* ======================== */
class PropulsionControl
{
private:
	vector<Motor*> Motors;
	double Direction[3];
public:

	PropulsionControl(string filename);
	void Run(void);

};

/* ======================== */
/* ======================== */
#endif
