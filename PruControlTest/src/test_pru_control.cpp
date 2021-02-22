/*****************************************************************************************************************
 * Copyright (c) 2021, Kenneth Lindsay
 * All rights reserved.
 * Author: Kenneth Lindsay
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
 *      following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *      following disclaimer in the documentation and/or other materials provided with the distribution.
 *   3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// Kernel version 4.19.94-ti-r42

/* This class requires that the permissions on the Beaglebone /lib/firmware be changed to
 * group write privileges.  In addition the ownership of the library /lib/firmware changed from
 * root:root to root:userxx.  This is done so that the pru code can be loaded into the firmware
 * library.
 *
 * $ sudo chown root:userxx /lib/firmware
 * $ sudo chmod g+w /lib/firmware
 *
 * With the kernel version 4.19.94-ti-r42
 * The pru code needs to be copied into the firmware library using the shell commands
 * $ cp ~/pru-code-xxx.out /lib/firmware/pru-code
 *
 * Enable PRU pins
 * $ config-pin P9_XX pruout
 *
 * Run application
 * $ echo /dev/remoteproc/pruss-core0 'pru-code' > firmware
 * $ echo /dev/remoteproc/pruss-core0 'start' > state
 * $ echo /dev/remoteproc/pruss-core0 'stop' > state
 *                      or
 * $ echo /dev/remoteproc/pruss-core1 'pru-code' > firmware
 * $ echo /dev/remoteproc/pruss-core1 'start' > state
 * $ echo /dev/remoteproc/pruss-core1 'stop' > state
 *
 *
 */

#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pru_control_class.h>

int handleLoadStateOptions(char *arg, PruControl *pru);
int handleCopyRestart(char *arg1, char *arg2, PruControl *pru);
int handleCore(char *arg1, PruControl *pru);
void argsUsage(const char *prog);

int main(int argc, char *argv[])
{
	PruControl pru;
	int rtnVal;
	switch(argc)
	{
		case 2: // Is argv a [ l | 1 | 0] no core specified if neither error
			pru.setCore(0);
			rtnVal = handleLoadStateOptions(argv[1], &pru);
			break;
		case 3: // Must be a 'c' or 'r' with no core specified or core specified with [l | 0 | 1]
			if(handleCore(argv[1], &pru))
				rtnVal = handleLoadStateOptions(argv[2], &pru);
			else
				rtnVal = handleCopyRestart(argv[1], argv[2], &pru);
			break;
		case 4: // Must be a 'c' or 'r' with core specified
			if(handleCore(argv[1], &pru))
				rtnVal = handleCopyRestart(argv[2], argv[3], &pru);
			break;
		default :
			rtnVal = -1;
	}
	if(rtnVal < 0)
		argsUsage(argv[0]);
	return rtnVal;

//	rtnVal = pru.copy("/home/pops/DHT_Sensor.out");
//	pru.load();
//	check = pru.checkLoad();
//	pru.start();
//	check = pru.checkRunState();
//	pru.stop();
//	check = pru.checkRunState();
	return 0;
}
int handleLoadStateOptions(char *arg, PruControl *pru)
{
	int rtnVal = 0;
	if(strcmp(arg, "-l") == 0)
		pru->load();
	else if(strcmp(arg, "-1") == 0)
		pru->start();
	else if(strcmp(arg, "-0") == 0)
		pru->stop();
	else
		rtnVal = -1;
 	return rtnVal;
}
int handleCopyRestart(char *arg1, char *arg2, PruControl *pru)
{
	int rtnVal = 0;
	std::string appPath;
	if(strcmp(arg1, "-c") == 0)
	{
		appPath.assign(arg2);
		pru->copy(appPath);
	}
	else if(strcmp(arg1, "-r") == 0)
	{
		appPath.assign(arg2);
		pru->copy(appPath);
		pru->load();
		pru->start();
	}
	else
		rtnVal = -1;
	return rtnVal;
}
int handleCore(char *arg1, PruControl *pru)
{
	int rtnVal = 1;
	if((strcmp(arg1, "0") == 0) || (strcmp(arg1, "1") == 0))
	{
		if(strcmp(arg1, "0") == 0)
		{
			pru->setCore(0);
		}
		else
			pru->setCore(1);
	}
	else
		rtnVal = 0;
	return rtnVal;
}

void argsUsage(const char *application)
{
	printf("Usage: %s \n", application);
	puts("Description: Used for controlling PRUS /dev/remoteproc/pruss-coreX\n"
         "    PRU_CORE [0 default | 1] required { [ OPTIONS ] }\n"
         "    OPTIONS:\n"
		 "      -c  { file path } copy file to \'/lib/firmware\' for loading as\n"
		 "          \'pru-user-app0\' (PRU_CORE = 0) or \'pru-user-app1\' (PRU_CORE = 1)\n"
		 "      -l  load PRU with \'/lib/firmware/pru-user-app0\' or \'pru-user-app1\'\n"
		 "          firmware file loaded is dependent on (PRU_CORE default = 0)\n"
		 "      -r  restart, copy file to \'/lib/firmware\' load and start\n"
		 "      -1  pru state set to \'running\' (start)\n"
		 "      -0  pru state set to \'offline\' (stop)\n");
	exit(1);
}



