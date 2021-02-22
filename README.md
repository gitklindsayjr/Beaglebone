# Beaglebone
This repository contains 3 Eclipse projects "Dht22Sensor", "PruControlTest", plus "PruLibrary".  The Eclipse projects were developed on a linux/Ubunt 18.04 as a Cross Compiled C++ project for the Beaglebone Black using "arm-linux-gnueabihf" tools.  The debugger used for these projects is "gdb-multiarch".  The tool path is using the "/usr/cross-pi-gcc-10.1.0-2/arm-linux-gnueabihf" which is also used for the RaspberryPI 4.

PruControlTest
--------------
This project builds a executable "pru-cntl" which is uploaded to the Beaglebone.  It utilizes the "/dev/remoteproc" to manage PRU0 or PRU1.  Refer to Derek Molloy's book Exploring Beaglebone Chapter 15, Real-Time Interfacing with the PRU-ICS.

Class methods:
-------------------------------------------
int PruControl.copy(std::string appPath)
  Copies the PRU obj code "xxxxxx.out" to /lib/firmware as "/lib/firmware/pru-user-app0" or "/lib/firmware/pru-user-app1" depending on which PRU is selected.
  Note: sudo priveleges for copy priveleges to "lib/firmware" must be used or changed to give user or group privleges with chmod.
void PruControl.load(void)
  Uses remoteproc to load the lib/firmware files "/lib/firmware/pru-user-app0" or "/lib/firmware/pru-user-app1"  depending on which PRU is selected
void PruControl.start(void);
  Uses remoteproc to start executing the previously loaded files "/lib/firmware/pru-user-app0" or "/lib/firmware/pru-user-app1"  depending on which PRU is selected
void PruControl.stop(void);
  Uses remoteproc to stop PRU execution
void PruControl.restart(void)
  Uses remoteproc to copy, load, and start selected PRU. Simplies development process.
  
pru-cntrl -- Command Line
------------
 Usage: pru-cntl
	Description: Used for controlling PRUS /dev/remoteproc/pruss-coreX
         PRU_CORE { 0 default | 1} required { { OPTIONS } }
         OPTIONS:
		-c  { file path } copy file to '/lib/firmware for loading as
		    pru-user-app0' (PRU_CORE = 0) or 'pru-user-app' (PRU_CORE = 1)
		-l  load PRU with '/lib/firmware/pru-user-app0' or 'pru-user-app1'
		    firmware file loaded is dependent on (PRU_CORE default = 0)
		 -r  restart, copy file to '/lib/firmware' load and start
		 -1  pru state set to 'running' (start)
		 -0  pru state set to 'offline' (stop)

Dht22Sensor
-----------------------------------
Eclipse cross compiled project for the Beaglebone that demonstrates the use of the BBB Prus's using the DHT22 Humidity, Temeprature sensor.  The  uploaded application "dht-sensor" supports up to 3 sensors connected to pre-defined pins, P8-11, P8-12, and P8-15.  Gpio pin P8-16 is used for debugging purpose when connected to a scope.

The application links the "pru-control_class.o" object form the "PruControlTest/Debug/src" path.  In addition it uses a 'S' character to synchronize RPMessage communication between the Beaglebone (HOST) and PRU app (DHT_Sensor.out).

dht22-sensor -- Command Line
-----------------------------------
userxxx.Beaglebone $ ./dht22-sensor

*************************************************************************************************************************************************
This section of the respository refers to the Code Composer Studio Version 10.2.0 project below. 
Follow this link for a good installation reference for CCS PRU development for the Beaglebone.

https://www.element14.com/community/community/designcenter/single-board-computers/next-genbeaglebone/blog/2019/05/14/coding-for-the-beaglebone-pru-with-c-in-2019

DHT_Sensor
--------------------------------------------------
This project was built using CCS V10.2. The code "dht_sensor.c" is dependent on the installation of the PRU resources installed during the CCS setup.

They are:
~/ti/oru-software-support-package
~/ti/oru-software-support-package/include
~/ti/oru-software-support-package/include/am335x
        --- And in my installation directory /opt/ti ---
/opt/ti/ccs1020/ccs/ccs_base/pru/include
/opt/ti/ccs1020/ccs/tools/compiler/ti=cgt-pru_2.3.3/include

  sources: dht_sensor.c, dht_sensor.h, pru_gpio.h, and ressource_table.h
  
Linker depencies require the PruLibrary project to be built and referenced for RPM_Message support

PruLibrary
-------------------------------
Build as a "Static Library" New CCS Project->Output type:

  sources: pru_rpmsg.c, pru_virtqueue.c
  

        
        

