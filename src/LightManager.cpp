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
#include <PWM_Controller.h>

#include "LightManager.h"

const char ParName[] = "LightCh";
//*******************************************************************************************
LightManager::LightManager(const JSON_Object *settings)
{
	OnOff = 0;

	if ( json_object_get_value(settings, ParName ) != NULL ) {
		Chanel = json_object_get_number(settings, ParName );
	} else {
		Chanel = -1;
	}
	printf("Light Chanel = %d\n", Chanel);
}

//*******************************************************************************************
void LightManager::Run(void)
{
	if ( OnOff ) {
		PWM_SetPWM(Chanel, 1.0);
	} else {
		PWM_SetPWM(Chanel, 0.0);
	}
}

//*******************************************************************************************
void LightManager::Toggle(void)
{
	if ( Chanel < 0 )
		return;

	OnOff ^= 1;

}

//*******************************************************************************************
//*******************************************************************************************


