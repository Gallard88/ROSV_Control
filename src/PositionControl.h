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
//*******************************************************************************************
#ifndef __POSITION_CONTROL__
#define __POSITION_CONTROL__
//*******************************************************************************************
#include "parson.h"

//*******************************************************************************************
typedef struct
{
    int ch;
    float scale[3];
    // forward
    // sideward
    // turn
    float power;
} PropulsionMotor;

//*******************************************************************************************
class PositionControl
{
public:
    PositionControl(const JSON_Object *settings);
    void Run(void);

    void Set_FwdVel(float vel);
    void Set_SwdVel(float vel);
    void Set_TurnVel(float vel);

    void Set_TargFwdVel(float vel);
    void Set_TargSwdVel(float vel);
    void Set_TargTurnVel(float vel);

private:

    float Targ_Fwd_Vel;
    float Targ_Swd_Vel;
    float Targ_Turn_Vel;


    PropulsionMotor *PropList;
    int Num_Prop;
};

//*******************************************************************************************
//*******************************************************************************************
#endif

