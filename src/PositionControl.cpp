/*
 Position Control ( http://www.github.com/Gallard88/ROSV_Control )
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

#include <string>
#include <iostream>
#include <cmath>

#include <syslog.h>
#include <PWM_Controller.h>

#include "PositionControl.h"

//*******************************************************************************************
PositionControl::PositionControl(const JSON_Object *settings)
{
    string var_name;
    JSON_Array *array;
    PropulsionMotor *new_mot;
    int rv, i;

    PropList = NULL;

    array = json_object_get_array( settings, "PropulsionMotor");
    if ( array == NULL )
    {
        syslog(LOG_EMERG,"Failed to find \"PropulsionMotor\" array in settings");
        return ;
    }

    rv = json_array_get_count(array);
    if ( rv == 0 )
        return ;
    PropList = new PropulsionMotor[rv];
    Num_Prop = rv;

    for ( i = 0; i < rv; i++)
    {
        new_mot = &PropList[i];

        const char *ptr = json_array_get_string(array, i);
        if ( ptr == NULL )
            continue;
        std::string name(ptr);

        new_mot->power = 0.0;

        var_name = name + "." + "ch";
        new_mot->ch = (int)json_object_dotget_number(settings, var_name.c_str());

        var_name = name + "." + "ratio";
        JSON_Array *scale = json_object_dotget_array(settings, var_name.c_str());
        if ( scale == NULL )
            continue;
        int size = json_array_get_count(scale);
        if ( size > 3 )
            size = 3;
        for ( int j = 0; j < size; j ++ )
        {
            new_mot->scale[j] = json_array_get_number(scale, j);
        }
    }
}

//*******************************************************************************************
void PositionControl::Run(void)
{
    PropulsionMotor *motor;
    int i;
    float max_power = 0;
    float scale = 1;

    if ( PropList == NULL )
        return;

    // Set power Levels
    for ( i = 0; i < Num_Prop; i++)
    {
        motor = &PropList[i];
        motor->power = 0;
        motor->power += Targ_Fwd_Vel * motor->scale[0];
        motor->power += Targ_Swd_Vel * motor->scale[1];
        motor->power += Targ_Turn_Vel * motor->scale[2];
        if ( abs(motor->power) > max_power )
            max_power = motor->power;
    }
    if ( max_power > 1 )
        scale = 1.0 / max_power;
    for ( i = 0; i < Num_Prop; i++)
    {
        motor = &PropList[i];
        PWM_SetPWM(motor->ch,  motor->power * scale );
    }
}

//*******************************************************************************************
void PositionControl::Set_TargFwdVel(float vel)
{
    Targ_Fwd_Vel = vel;
    Run();
}

//*******************************************************************************************
void PositionControl::Set_TargSwdVel(float vel)
{
    Targ_Swd_Vel = vel;
    Run();
}

//*******************************************************************************************
void PositionControl::Set_TargTurnVel(float vel)
{
    Targ_Turn_Vel = vel;
    Run();
}

//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************

