
using namespace std;

#include <stdio.h>
#include <string>
#include <string.h>

#include "motor.h"
/* ======================== */
/* ======================== */
Motor::Motor(JSON_Object *obj, string name)
{
	/*
	 * Parse JSON object for required fields.
	 *
	 * motor ch
	 * scale
	 */
	string search;

	Name = name;

	search = name + ".scale_0";
  Scale_Factor[0] = json_object_get_number(obj, search.c_str());

	search = name + ".scale_1";
  Scale_Factor[1] = json_object_get_number(obj, search.c_str());

	search = name + ".scale_2";
  Scale_Factor[2] = json_object_get_number(obj, search.c_str());

	search = name + ".max";
  MaxPower = (int) json_object_get_number(obj, search.c_str());
}

/* ======================== */
double Motor:: GetPower(double *direction)
{
	double power = 0;

	for ( int i = 0; i < 3; i ++ )
	{
		power += direction[i] * Scale_Factor[i] * MaxPower;
	}

	return power;
}

/* ======================== */
/* ======================== */

