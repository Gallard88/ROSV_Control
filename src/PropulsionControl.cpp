
/* ======================== */
using namespace std;

#include <stdio.h>
#include <string>
#include <string.h>

#include "PropulsionControl.h"

/* ======================== */
PropulsionControl::PropulsionControl(string filename)
{
  int rv, i;
  JSON_Value * val;
  JSON_Object *object;
  JSON_Array *array;

	val = json_parse_file(filename.c_str());
	rv = json_value_get_type(val);
  if ( rv != JSONObject ) {
		printf("System didn't work, %d\n", rv );
		return;
  }

  object = json_value_get_object(val);
	if ( object == NULL ) {
		printf("Onject == NULL\n");
		return;
	}

	array = json_object_get_array(object, "motors");
	if ( array == NULL)
	{
		printf("array == NULL\n");
		return;
	}

	rv = json_array_get_count(array);
	printf("Array Count %d\n", rv );
	for ( i = 0; i < rv; i++)
	{
		Motor *motor;
		const char *ptr;

		ptr = json_array_get_string(array, i);
		if ( ptr == NULL )
			continue;
		std::string name(ptr);

		motor = new Motor(object, name);

		Motors.push_back(motor);
  	printf("Motor %s added\n", name.c_str());
	}
}

/* ======================== */
void PropulsionControl::Run(void)
{
	for ( size_t i = 0; i < Motors.size(); i ++ )
	{
		Motor *m;
		m = Motors[i];
		double power = m->GetPower(Direction);
		printf("Motor: %s, %f\n", m->Name.c_str(), power);
		// do something with this data.
	}
}

/* ======================== */

/* ======================== */
/* ======================== */
