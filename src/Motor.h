/*
 Motor ( http://www.github.com/Gallard88/ROSV_Control )
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
#ifndef __MOTOR__
#define __MOTOR__
//*******************************************************************************************
#include <parson.h>
#include <string>

//*******************************************************************************************
#define VECTOR_SIZE     6

//*******************************************************************************************

class  Motor {
public:
  Motor(const JSON_Object *setting);
  virtual ~Motor();

  bool Run(const float *power);

  float GetPower(void);
  int GetChanel(void);

  std::string GetJSON(void);

  static void SetRamp(float ramp);

private:
  int Chanel;
  int mult[VECTOR_SIZE];
  float Value;
  float LastValue;
  std::string Name;

  static float Ramp;
};


//*******************************************************************************************
//*******************************************************************************************
#endif

