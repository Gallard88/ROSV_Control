/* ======================== */
using namespace std;

#include <iostream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include "TcpServer.h"
#include "CmdStream.h"
#include "DataLogger.h"	// for test purposes only

/* ======================== */
const string prop_file = "./ROSV_Motor.json";
#define SYSTEM_DELAY	100
const struct timeval system_time = {10,SYSTEM_DELAY};

/* ======================== */
volatile bool Run_Control;

#define UV_INTERVAL 100000	// 100 ms

/* ======================== */
TcpServer Listner(1);

/* ======================== */
void alarm_wakeup (int i)
{
   struct itimerval tout_val;

   signal(SIGALRM,alarm_wakeup);

//   printf("Tick Tock\n");
   tout_val.it_interval.tv_sec = 0;
   tout_val.it_interval.tv_usec = 0;
   tout_val.it_value.tv_sec = 0; // 10 seconds timer
   tout_val.it_value.tv_usec = UV_INTERVAL;

   setitimer(ITIMER_REAL, &tout_val,0);

	 Run_Control = true;

}

/* ======================== */
int main (int argc, char *argv[])
{
	// load paramaters
	// open logging module

	// open TCP server
	if ( Listner.Connect(8090) < 0)
	{
		cout << "Listner Failed" << endl;
		return -1;
	}
	cout << "Listner setup" << endl;
	// start command processor

	// set up timer signal and signal handler
	alarm_wakeup(0);


	// Starting the control module
//	prop_con = new PropulsionControl(prop_file);
	Listner.CheckNewConnetions();

	cout << "Starting Main Program" << endl;

	while (1)
	{
		struct timeval timeout = system_time;
		string cmd, arg;

		// read data from connected clients.
		Listner.Run(&timeout);
		while ( Listner.ReadLine(&cmd, &arg) > 0)
			cout << "Cmd: " << cmd << " Arg: " << arg << endl;

		if ( Run_Control == true)
		{
			Run_Control = false;
//			prop_con->Run();
		}
	}

	return 0;
}

/* ======================== */
/* ======================== */
