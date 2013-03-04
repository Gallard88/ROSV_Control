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

//*******************************************************************************************
//*******************************************************************************************
#ifndef __DEPTH_MANAGER__
#define __DEPTH_MANAGER__
//*******************************************************************************************

#include "parson.h"
#include "PWM_Con.h"

//*******************************************************************************************
typedef struct
{
	int ch;
	float pitch_ratio;
	float roll_ratio;
	float power;
} DepthMotor;

//*******************************************************************************************
class DepthManager
{
public:
	DepthManager(const JSON_Object *settings, PWM_Con *pwm);
	void Run(void);
	void Enable(void);
	void Disable(void);
	void SetDepthPower(float power);
	void SetPitch_Roll(float pitch, float roll);
	void SetTargetPitch_Roll(float pitch, float roll);

private:
	PWM_Con *Pwm;

	float Depth_Power;
	float Pitch, Roll;
	float Targ_Pitch, Targ_Roll;
	bool DiveEnable;

	DepthMotor *MotorList;
	int Num_Motor;
};

//*******************************************************************************************
//*******************************************************************************************
#endif

