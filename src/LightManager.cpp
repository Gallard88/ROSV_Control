/*
 Light Manager ( http://www.github.com/Gallard88/ROSV_Control )
 Copyright (c) 2013 Thomas BURNS

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

//*******************************************************************************************
using namespace std;

#include <syslog.h>

#include <stdio.h>
#include <string.h>
#include <string>

#include "LightManager.h"
#include "parson.h"

//*******************************************************************************************
LightManager::LightManager(const char * filename)
{
  JSON_Value *val = json_parse_file(filename);
  int rv = json_value_get_type(val);

  if ( rv != JSONObject ) {
    syslog(LOG_EMERG, "Lighting: JSON Parse file failed\n");
		json_value_free (val);
    return;
  }

  JSON_Object *settings = json_value_get_object(val);
  if ( settings == NULL ) {
    syslog(LOG_EMERG, "Lighting: Settings == NULL\n");
		json_value_free (val);
    return;
  }
  JSON_Array *ch_array = json_object_get_array( settings, "Modules");
  if ( ch_array == NULL ) {
    syslog(LOG_EMERG,"Lighting: Failed to find \"Modules\" array in settings");
		json_value_free (val);
    return ;
  }

  size_t number = json_array_get_count(ch_array);
  if ( number != 0 ) {
		for ( size_t i = 0; i < number; i ++) {
			LightChanel ch;
			ch.Power = 1;
			ch.Modules.clear();

			JSON_Object *object = json_array_get_object (ch_array, i);
			if ( object == NULL ) {
				return;
			}
			const char *name = json_object_get_string (object, "Name");
			if ( name != NULL ) {
				ch.Name = string(name);
			}

			JSON_Array *pwm_array = json_object_get_array( object, "Chanels");
			if ( pwm_array != NULL ) {
				for ( size_t j = 0; j < json_array_get_count(pwm_array); j++ ) {
					int value = (int) json_array_get_number (pwm_array, j);
					ch.Modules.push_back(value);
				}
			}
			Chanels.push_back(ch);
		}
	}
  json_value_free (val);
}

//*******************************************************************************************
void LightManager::Run(void)
{/* The PWM has a ~7 second watchdog.
	*	If this function is run once per second it should be fast enough
	*/
	time_t current;
	current = time(NULL);
	if ((current - update) > 1) {
		update = current;

		for ( size_t i = 0; i < Chanels.size(); i ++ ) {
			LightChanel ch = Chanels[i];
			for ( size_t j = 0; j < ch.Modules.size(); j ++ ) {
				PWM_SetPWM(Pwm, ch.Modules[j], ch.Power);
			}
		}
	}
}

//*******************************************************************************************
void LightManager::Update(const string & msg)
{
}

//*******************************************************************************************
const string LightManager::GetConfigData(void)
{
	string msg("{ \"RecordType\": \"LightCfg\", ");

	msg += "\"Chanels\":[ ";
	for ( size_t i = 0; i < Chanels.size(); i ++ ) {
		msg += " \"";
		msg += Chanels[i].Name;
		msg += "\"";
		if ( Chanels.size() > 1 ) {
			msg += ", ";
		}
	}
	msg += " ]}";
	return msg;
}

//*******************************************************************************************
const string LightManager::GetData(void)
{
	char power[10];
	string msg("{ \"Module\": \"LightData\", ");
	msg += "\"Chanels\":[ ";
	for ( size_t i = 0; i < Chanels.size(); i ++ ) {
		msg += " {\"Name\": \"";
		msg += Chanels[i].Name;
		msg += "\", \"Power\": ";
		sprintf(power, "%d", Chanels[i].Power);
		msg += string(power);
		msg += "}";
		if ( Chanels.size() > 1 ) {
			msg += ", ";
		}
	}
	msg += " ]}";
	return msg;
}

//*******************************************************************************************
//*******************************************************************************************


