/*
 PWM Control ( http://www.github.com/Gallard88/ROSV_Control )
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


// *****************
#define _XOPEN_SOURCE

// *****************
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

using namespace std;

#include "PWM_Con.h"

// *******************************************************************************************
PWM_Con::PWM_Con(void)
{
//	if ( PWM_mem == NULL )
  {
    int shmid;

    // Make sure the file exists.
    int fd = open(PWM_KEY_FILE, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    /* Only wanted to make sure that the file exists. */
    close(fd);

    // Generate memory key. */
    key_t key = ftok(PWM_KEY_FILE, PWM_MEM_KEY);
    if (key	 == -1)
    {
      perror("ftok");
      exit(1);
    }

    // connect to (and possibly create) the segment:
    if ((shmid = shmget(key, PWM_CON_SHM_SIZE, O_RDWR)) == -1)
    {
      perror("shmget");
      exit(1);
    }

    // attach to the segment to get a pointer to it:
    PWM_mem = (Pwm_Con_Mem *)shmat(shmid, (void *)0, 0);
    if ((char *)PWM_mem == (char *)(-1))
    {
      perror("shmat");
      exit(1);
    }
  }
}

//*******************************************************************************************
void PWM_Con::SetLevel(int ch, float level)
{
  if ( level > 1.0 )
    level = 1.0;
  if ( level < 0.0 )
    level = 0.0;
  if ( ch >= PWM_NUM_CHANELS )
    return ;

  pthread_mutex_lock( &PWM_mem->access );

  PWM_mem->ch[ch] = level;
  PWM_mem->data_ready = 1;

  pthread_mutex_unlock( &PWM_mem->access );
}

//*******************************************************************************************
float PWM_Con::GetTemp(void)
{
  float temp;

  pthread_mutex_lock( &PWM_mem->access );
  temp = PWM_mem->temperature;
  pthread_mutex_unlock( &PWM_mem->access );

  return temp;
}

//*******************************************************************************************
float PWM_Con::GetVolt(void)
{
  float voltage;

  pthread_mutex_lock( &PWM_mem->access );
  voltage = PWM_mem->voltage;
  pthread_mutex_unlock( &PWM_mem->access );

  return voltage;
}

//*******************************************************************************************
float PWM_Con::GetCurrent(void)
{
  float current;

  pthread_mutex_lock( &PWM_mem->access );
  current = PWM_mem->current;
  pthread_mutex_unlock( &PWM_mem->access );

  return current;
}

//*******************************************************************************************
//*******************************************************************************************

