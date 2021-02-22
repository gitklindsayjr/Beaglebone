/*
 * pru_load_class.h
 *
 *  Created on: Jan 22, 2021
 *      Author: pops
 */

#ifndef PRU_CONTROL_CLASS_H
#define PRU_CONTROL_CLASS_H

#include <string>
#include <cstring>

#define DEV_PATH_PRU0   "/dev/remoteproc/pruss-core0"
#define DEV_PATH_PRU1   "/dev/remoteproc/pruss-core1"
#define LIB_COPY_PATH   "/lib/firmware"

class PruControl
{
	public:
		PruControl(void) { };
		PruControl(int pruCore);
		void load(void);
		void start(void);
		void stop(void);
		void restart(void);
		int  copy(std::string appPath);
		int  getCore(void) { return pruCore; }
		bool checkRunState(void);
		bool checkLoad(void);
		bool setCore(int pruCore);
	private:
		void setDevicePath(int load);
		void closePru(void) { close(dev); devPath = ""; }
		bool checkCore(int pruCore);
		int  openPru(int type);
		int  writePru(char *str, int size);
		int  readPru(char *str, int size);
		int pruCore = 0;
		std::string devPath = "";
		int dev = 0;
};

#endif // PRU_CONTROL_CLASS_H
