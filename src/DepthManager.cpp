/*
 Depth Manager ( http://www.github.com/Gallard88/ROSV_Control )
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

// *******************************************************************************************
using namespace std;
#include <string>
#include <iostream>

#include <syslog.h>
#include <PWM_Controller.h>

#include "DepthManager.h"

// *******************************************************************************************
DepthManager::DepthManager(const JSON_Object *settings)
{
    string var_name;
    JSON_Array *array;
    DepthMotor *new_mot;
    int rv, i;

    DiveEnable = false;
    MotorList = NULL;

    array = json_object_get_array( settings, "DepthMotor");
    if ( array == NULL )
    {
        syslog(LOG_EMERG, "Failed to find \"DepthMotor\" array in settings file");
        return ;
    }

    rv = json_array_get_count(array);
    if ( rv <= 0 )
        return ;
    Num_Motor = rv;
    MotorList = new DepthMotor[rv];

    for ( i = 0; i < rv; i++ )
    {
        new_mot = &MotorList[i];

        const char *ptr = json_array_get_string(array, i);
        if ( ptr == NULL )
            continue;
        std::string name(ptr);

        new_mot->power = 0.0;

        var_name = name + "." + "ch";
        new_mot->ch = (int)json_object_dotget_number(settings, var_name.c_str());
    }
}

//*******************************************************************************************
void DepthManager::Run(void)
{
    DepthMotor *motor;
    int i;

    if ( DiveEnable == false )
        return ;
    if ( MotorList == NULL )
        return;

    for ( i = 0; i < Num_Motor; i++)
    {
        motor = &MotorList[i];
        PWM_SetPWM(motor->ch, Depth_Power );
    }
}

//*******************************************************************************************
void DepthManager::Enable(void)
{
    DiveEnable = true;
}

//*******************************************************************************************
void DepthManager::Disable(void)
{
    Depth_Power = 0;
    Run();
}

//*******************************************************************************************
void DepthManager::SetDepthPower(float power)
{
    if ( DiveEnable )
        Depth_Power = power;
}

//*******************************************************************************************
void DepthManager::SetPitch_Roll(float pitch, float roll)
{
    Pitch = pitch;
    Roll = roll;
}

//*******************************************************************************************
void DepthManager::SetTargetPitch_Roll(float pitch, float roll)
{
    Targ_Pitch = pitch;
    Targ_Roll = roll;
}

//*******************************************************************************************
//*******************************************************************************************

