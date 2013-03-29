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

#include <stdio.h>
#include <string.h>
#include <string>
#include <PWM_Controller.h>

#include "LightManager.h"

//*******************************************************************************************
LightManager::LightManager(const JSON_Object *settings)
{
    struct LightCh *new_ch;
    JSON_Array *array;
    int i;

    array = json_object_get_array(settings, "Lights");
    if (array == NULL )
    {
        printf( "LM Error, \"Lights\" array not found\n");
        return;
    }

    Num_Chanels = json_array_get_count(array);
    if ( Num_Chanels <= 0 )
    {
        printf("Empty Array\n");
        return ;
    }
//  printf("%d Light chanels\n", Num_Chanels);
    Chanels = new struct LightCh[Num_Chanels];

//  printf ( "Parsing Settings\n");
    for ( i = 0; i < Num_Chanels; i ++ )
    {
        new_ch = &Chanels[i];
        string search;
        const char *ptr = json_array_get_string(array, i);
        if ( ptr == NULL )
            continue;
        strncpy(new_ch->name, ptr, LM_NAME_SZE);

        std::string name(ptr);
        search = name + ".ch";
        new_ch->ch = (int)json_object_dotget_number(settings, search.c_str());
//    printf("%s %d\n", search.c_str(), new_ch->ch);

        search = name + ".max";
        new_ch->max = json_object_dotget_number(settings, search.c_str());
//    printf("%s, %f\n", search.c_str(), new_ch->max);

        search = name + ".min";
        new_ch->min = json_object_dotget_number(settings, search.c_str());
//    printf("%s, %f\n",search.c_str(), new_ch->min);

//    printf("%s, %s\n", search.c_str(), new_ch->name);

//    printf("Next Ch\n");
    }
//  printf("Light Manager setup\n");
}

//*******************************************************************************************
void LightManager::SetBrightness(const char *ch, const float level)
{
    float power;
    struct LightCh *ptr;
    int i;

    for ( i = 0; i < Num_Chanels; i ++ )
    {
        ptr = &Chanels[i];
        if ( strcmp(ptr->name, ch) == 0)
        {
            if ( level > ptr->max)
                power = ptr->max;
            else if ( level < ptr->min )
                power = ptr->min;
            else
                power = level;
//      printf ("Ch %s = %f\n", ch, power);
            PWM_SetPWM(ptr->ch, power);
        }
    }
}

//*******************************************************************************************
//*******************************************************************************************


