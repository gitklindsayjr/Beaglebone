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
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "pru_control_class.h"

#define SENSOR_BYTES    5

static PruControl pru = PruControl(0);
static bool exitFlag = false;
static void sigHandler(int sigNum);

int main(int argc, char **argv)
{
	signal(SIGINT, sigHandler);        // Trap keyboard interrupt ctrl-c to quit app
	/* Load DHT PRU application "DHT_Sensor.out" by copying to /lib/firmware.  File in
	 * /lib/firmware will be named "pru-user-core0".  This file is then loaded into the
	 * device "/dev/remoteproc/pruss-core0/firmware" as "pru-user-core0".  This is done
	 * with pru.restart() function. In addition a "start" command is written to the file
	 * "/dev/remoteproc/pruss-core0/state.  This starts the PRU application which will be in
	 * a wait state looking for a 'S' character to be sent.  The 'S' will then be echoed back
	 * to this app.
	 ****************************************************************************************/
	if(pru.copy("/home/pops/DHT_Sensor.out") < 0)
		return 0;
	pru.restart(); // Load and start PRU app
	sleep(1);      // Wait 1 second for PRU to startup and look for 'S'
	int port;                             // DHT22 read/write device descriptor
	uint8_t resultBuffer[SENSOR_BYTES + 1];  // DHT22 data buffer,
	port = open("/dev/rpmsg_pru30", O_RDWR); // Open rmpmsg port
	if(port < 0)
		{ perror("ERROR: Can't open /dev/rpmsg_pru30"); return -1; }
	if(write(port, "S", 1) != 1)   // Send 'S' to start
		{ close(port); return 0; } // If not sent close this program
	while(true)
	{   // Loop until the PRU echos back the 'S'
		read(port, resultBuffer, sizeof(resultBuffer)); // Blocks until data received
		if(resultBuffer[0] == 'S') // We are now ready
			break;
	}
	int     pin;
	int16_t rawTemp, rawHum;
	uint8_t chkSum = 0;
	float   tempC, tempF, humidity;
	while(!exitFlag)
	{   // Loop until ctrl-c signal sets exit flag to true
		read(port, resultBuffer, SENSOR_BYTES + 1); // Grab a buffer full of data
		pin = resultBuffer[SENSOR_BYTES];
		chkSum = 0;                                 // Initialize checksum value
		for(int i=0; i<(SENSOR_BYTES - 1); i++)       // Calculate the checksum
			chkSum+= resultBuffer[i];
		if(chkSum != resultBuffer[SENSOR_BYTES - 1]) // If checksum fails skip
			continue;
		rawHum  = (resultBuffer[0]<<8) + resultBuffer[1];
		rawTemp = (resultBuffer[2]<<8) + resultBuffer[3];
		humidity = rawHum/10.0;
		tempC = rawTemp/10.0;
		tempF = 9*tempC/5 + 32;
		printf("Pin: %d", pin);
		printf(", Temperature: %.1f \xC2\xB0 C", tempC); // Prints to ccs console
		printf(" | %.1f \xC2\xB0 F", tempF);
		printf(" | Humidity: %.1f\n", humidity);
		sleep(1);                      // The fastest data rate is 10ms or 100 samples/second
		if(write(port, "S", 1) != 1)   // Send 'S' to start
			{ close(port); return 0; } // If not sent close this program
	}
	close(port); // Exit flag is true close port and
	pru.stop();  // Stop PRU
	puts("\n");  // Terminate with final new line
	return 0;
}
static void sigHandler(int sigNum)
{   // Signal handler to  trap keyboard ctrl-c
	if(sigNum == SIGINT)
		exitFlag = true;
}



