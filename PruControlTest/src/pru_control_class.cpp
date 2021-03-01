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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "pru_control_class.h"

#define PRU_PROC_CORE0        "/dev/remoteproc/pruss-core0"
#define PRU_PROC_CORE1        "/dev/remoteproc/pruss-core1/"
#define STATE_STR_RUNNING     "running"
#define STATE_STR_OFFLINE     "offline"
#define FIRMWARE_STR         "/firmware"
#define STATE_STR             "/state"
#define APP_STR_CORE0         "pru-user-app0"
#define APP_STR_CORE1         "pru-user-app1"

enum file_t { LOAD, STOP, START };

PruControl::PruControl(int pruCore)
{
	if(checkCore(pruCore))
		this->pruCore = pruCore;
}
void PruControl::restart(void)
{
	load();
	start();
}
bool PruControl::checkRunState(void)
{
	if(openPru(START) < 0)
		return false;
	char state[] = STATE_STR_RUNNING; // "running" or "offline"
	int n = strlen(state);
	readPru(state, n);
	closePru();
	n = strcmp(state,STATE_STR_RUNNING );
	if(n == 0)
		return true;
	return false;
}
bool PruControl::checkLoad(void)
{   // Returns true if core0 or core1
	if(openPru(LOAD) < 0)
		return false;
	char app[] = APP_STR_CORE0; // "pru-user-app0"
	if(pruCore == 1)
		strcpy(app, APP_STR_CORE1);
	int n;
	n = strlen(app);
	readPru(app, n);
	closePru();
	if(pruCore == 0)
		n = strcmp(app, APP_STR_CORE0);
	else
		n= strcmp(app, APP_STR_CORE1);
	if(n == 0)
		return true;
	return false;
}

bool PruControl::checkCore(int pruCore)
{
	if(pruCore != 0 && pruCore != 1)
	{
		printf("ERROR: PRU Core number needs to be a 0 or 1\n");
		return false;
	}
	return true;
}
bool PruControl::setCore(int pruCore)
{
	if(checkCore(pruCore))
	{
		this->pruCore = pruCore;
		return true;
	}
	return false;
}
int PruControl::copy(std::string appPath)
{
	FILE *lib, *app;
	if(pruCore == 0)
	{
		lib = fopen("/lib/firmware/pru-user-app0", "w");
		if(lib == NULL)
		{
			perror("ERROR: Can't open /lib/firmware/pru-app0");
			return -1;
		}
	}
	else if(pruCore == 1)
	{
		lib = fopen("/lib/firmware/pru-user-app1", "w");
		if(lib == NULL)
		{
			perror("ERROR: Can't open /lib/firmware/pru-app1");
			return -1;
		}

	}
	else
	{
		printf("ERROR: PRU Core number needs to be a 0 or 1\n");
		return -1;
	}
	app = fopen(appPath.c_str(), "r");
	if(app == NULL)
	{
		printf("ERROR: Can't open %s", appPath.c_str());
		fflush(stdout);
		perror(" ");
		fclose(lib);
		return -1;
	}
	int byte, rtnVal;
	do
	{
		rtnVal= fgetc(app);
		byte = rtnVal;
		if(rtnVal != EOF)
		{
			rtnVal = fputc(byte, lib);
		}
	}while(rtnVal != EOF);
	fclose(lib);
	fclose(app);
	return 0;
}
void PruControl::load(void)
{
	stop();
	char app[] = APP_STR_CORE0;
	if(pruCore == 1)
		strcpy(app, APP_STR_CORE1);
	if(openPru(LOAD))
		return;
	int n = strlen(app);
	n = writePru(app, n);
	if(n != (int)strlen(app))
	{
		printf("ERROR: failed to set app %s/%s", devPath.c_str(), app);
		fflush(stdout);
		perror(" ");
	}
	closePru();
}
void PruControl::start(void)
{
	char state[] = STATE_STR_RUNNING; // "running or "offline"
	if(openPru(START) < 0)
		return;
	int n = strlen(state);
	readPru(state, n);
	n = strcmp(state, STATE_STR_RUNNING);
	if(n == 0)
	{   // Running already
		closePru();
		return;
	}
	strcpy(state, "start");
	n = strlen(state);
	n = writePru(state, n);
	if(n < 0)
	{
		closePru();
		return;
	}
	if(n != (int)strlen("start"))
	{
		printf("ERROR: Failed to set state %s/%s", devPath.c_str(), state);
		fflush(stdout);
		perror(" ");
	}
	closePru();
}
void PruControl::stop(void)
{
	char state[] = STATE_STR_OFFLINE; // "running or "offline"
	if(openPru(STOP) < 0)
		return;
	int n = strlen(state);
	readPru(state, n);
	n = strcmp(state, STATE_STR_OFFLINE);
	if(n == 0)
	{   // Offline alleady
		closePru();
		return;
	}
	strcpy(state, "stop");
	n = strlen(state);
	n = writePru(state, n);
	if(n < 0)
	{
		closePru();
		return;
	}
	if(n != strlen("stop"))
	{
		printf("ERROR: Failed to set state %s/%s", devPath.c_str(), state);
		fflush(stdout);
		perror(" ");
	}
	closePru();
}
int PruControl::openPru(int type)
{
	setDevicePath(type);
	dev = open(devPath.c_str(), O_RDWR);
	if(dev < 0)
	{
		printf("ERROR: Can't open %s", devPath.c_str()); fflush(stdout); perror(" ");
		return -1;
	}
	this->dev = dev;
	return 0;
}
int PruControl::writePru(char *str, int size)
{
	int rtnVal = 0;
	rtnVal = write(dev, str, size);
	if(rtnVal < 0)
	{
		printf("ERROR: Write failed - %s/%s", devPath.c_str(), str);
		fflush(stdout);	perror(" ");
	}
	return rtnVal;
}
int PruControl::readPru(char *str, int size)
{
	int rtnVal = 0;
	rtnVal = read(dev, str, size);
	if(rtnVal < 0)
	{
		printf("ERROR: Read failed - %s/%s", devPath.c_str(), str);
		fflush(stdout);
		perror(" ");
	}
	return rtnVal;
}
void PruControl::setDevicePath(int load)
{
	if(pruCore == 0)
		devPath.assign(PRU_PROC_CORE0);
	else
		devPath.assign(PRU_PROC_CORE1);
	switch(load)
	{
		case LOAD:
			devPath.append(FIRMWARE_STR);
			break;
		case START:
		case STOP:
			devPath.append(STATE_STR);
			break;
		default:
			devPath.clear();
			break;
	}
}



