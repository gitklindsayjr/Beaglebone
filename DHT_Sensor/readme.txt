Refer to TI Documentation

PRU Training: Hands-on Labs


The folder PRU_src is built usin CCS 10.2 and uploaded to the Beaglebone, as DHT_Sensor.out.
The pru app supports 3 sensors.  Wiring and GPIO pinout information is contained within the
"dht_sensor.c" file.
 
Use the application "pru-cntl" built and uploaded to the Beaglebone from project "PruControlTest".
This executable copys PRU executables to /lib/firmware for loading via remoteproc calls.

Also upload the program "dht22-sensor" built from the "Dht22Sensor" project to the Beaglebone.
This is the Linux Host executable that receives RPMessages from PRU0 and prints temperature and
humidity data to the Beaglebone console.

Beaglebone Black Kernel Version 4.19.94-ti-r42

Command line use:
$ ./pru-cntl -r DHT_Sensor.out   # copys, loads, and starts the DHT_Sensor.out pru executable
$ ./dht22-sensor                 # synchronizes PRU output and displays results



