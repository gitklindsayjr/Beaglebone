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

/* To run use following shell commands
        $ cp DHT_Sensor.out /lib/firmware/dht-sensor # after chown and chmod privileges set
        $ echo 'echo-messages' > /sys/class/remoteproc/remoteproc1/firmware
        $ echo 'start' > /sys/class/remoteproc/remoteproc1/state

        $ echo "test30: > /dev/rpmsg_pru30
        $ cat /dev/rpmsg_pru30 # curl-c to close cat

---- OR ------
        Use the executable "pru-cntl" crated from the Eclipse Project "PruControlTest"

        $ ./pru-cntl 0 -r DHT_sensor.out  # Copies, loads pru CORE0 and starts pru running
*/
/*********************************************************************************
*         BEAGLEBONE BLACK                                        -------------
*         -------------------                                     |           |
*         |                 |                                     |  Putty    |
*         |        ETHERNET |-----<-<---------------------> >-----|  (SSH)    |
*         |                 |                                     |           |
*         |                 |                                     -------------
*         |                 |                                     ---------------
*         |           3.3V  |---> P9-5 ----------------------> >--| 1  5.0-3.3V |
*         |           3.3V  |---> P9-3   |                        |             |
*         |           5.0V  |---> P9-6   |   --------             |             |
*         |           5.0V  |---> P9-5   ----| 4.7k |-----        |    DHT22    |
*         |                 |                --------    |        |             |
*         |                 |                            |        |             |
*         |           P8_12 |---> P8-11 <--------------------> <--| 2  Signal   |
*         |                 |                                     |             |
*         |           P8_11 |---> P8-12                           |             |
*         |                 |                                     |             |
*         |           P8_15 |---> P8-15                           |             |
*         |                 |                                     |             |
*         |           P8_16 |---> P8_16 To scope                  | 3 GND       |
*         |                 |                                     |             |
*         |             GND |---> P9-2 >---------------------> >--| 4  GND      |
*         -------------------                                     ---------------
*
**********************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>
#include "dht_sensor.h"
#include "pru_gpio.h"
#include "resource_table_0.h"

#define DHT0_PIN                GPIO1_P8_11_BIT13
#define DHT1_PIN                GPIO1_P8_12_BIT12
#define DHT2_PIN                GPIO1_P8_15_BIT15
#define DEBUG_PIN               GPIO1_P8_16_BIT14

/* Setup memory pointers */
volatile far pruGPIO *GPIO1 = (volatile pruGPIO *)(GPIO1_OFFSET);
volatile far pruGPIO *bank =  (volatile pruGPIO *)(GPIO1_OFFSET);
volatile far uint8_t *sharedMem = (volatile uint8_t *)(PRU_SHARED_RAM);

uint8_t  dataBuffer[SENSOR_BYTES + 1];
uint16_t src, dst;

static void initializeMessage(void); // Initialize RPM Message
static void waitForStart(void);      // Wait for 'S' echo response from PRU to HOST
static void synchronize(void);       // Synchronize PRU to HOST, PRU waits for 'S' from HOST
static bool start(void);             // Start DHT22 transmission
static bool readBit(void);           // Read one bit
static uint8_t read(void);           // Read a byte
static void readSensor(void);        // Read all 5 sensor bytes, 2 Hum., 2 Temp., 1 Checksum
volatile unsigned pin = 0;           // GPIOx bit selection variable

struct pru_rpmsg_transport transport;

void main(void)
{
    volatile unsigned pinIndex = 0; // Iterator for DHT selection
    volatile bool pin0 = false;     // DHT0 pin connection assignment
    volatile bool pin1 = false;     // DHT1 pin connection assignment
    volatile bool pin2 = false;     // DHT2 pin connection assignment

    /* Allow OCP master port access by the PRU so the PRU can read external memories */
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
    initializeMessage();
    /* For debugging purposes use this pin on a scope */
    GPIO1->DATAOUT&= ~DEBUG_PIN;  // Set Debug pin LOresultBuffer
    GPIO1->OE&= ~DEBUG_PIN;       // Set Debug pin as output

    /* Setup the GPIO pin assignments Look for input LO for unassigned pins.
     * Assigned pins are pulled HI. */
    GPIO1->OE|= DHT0_PIN; // Set pin to input
    GPIO1->OE|= DHT1_PIN; // Set pin to input
    GPIO1->OE|= DHT2_PIN; // Set pin to input

    __delay_cycles(100*CLKFREQ); // Delay for 100us, pins float HI
    if((bank->DATAIN&DHT0_PIN) == DHT0_PIN)
    {   // Do we have a DHT connected to this pin
        pin0 = true;                // Yes, DHT0
        bank->DATAOUT|= DHT0_PIN;   // Preset pin HIuint16_t
        bank->OE&= ~DHT0_PIN;       // Set DHT pin as output
    }
    if((bank->DATAIN&DHT1_PIN) == DHT1_PIN)
    {   // Do we have a DHT connected to this pin
        pin1 = true;
        bank->DATAOUT|= DHT1_PIN;   // Preset pin HI
        bank->OE&= ~DHT1_PIN;       // Set DHT pin as output
    }

    if((bank->DATAIN&DHT2_PIN) == DHT2_PIN)
    {   // Do we have a DHT connected to this pin
        pin2 = true;
        bank->DATAOUT|= DHT2_PIN;   // Preset pin HI
        bank->OE&= ~DHT2_PIN;       // Set DHT pin as output
    }
    waitForStart(); // Pauses here until host is ready
    pinIndex = pin = 0;
    while(true)
    {   // Sequence through available devices
        pinIndex++;
        switch(pinIndex)
        {
            case 1:
                if(pin0)
                    pin = DHT0_PIN;
                break;
            case 2:
                if(pin1)
                    pin = DHT1_PIN;
                break;
            case 3:
                if(pin2)
                    pin = DHT2_PIN;
                break;
            case 4:
            default:
                pinIndex = 0;
                continue;
        }
        if(pin == 0)
        {   // Failure, Toggle debug to indicate no sensors detected
            GPIO1->DATAOUT^= DEBUG_PIN;
            __delay_cycles(100*CLKFREQ);
            continue;
        }
        if(start())
        {   // Wait for valid DHT start of transmission
            GPIO1->DATAOUT|= DEBUG_PIN;  // Debug signal HI to scope if required
            readSensor();
            GPIO1->DATAOUT&= ~DEBUG_PIN; // Debug signal LO to scope if required
            if(pin == DHT0_PIN)
                dataBuffer[SENSOR_BYTES] = 0;
            if(pin == DHT1_PIN)
                dataBuffer[SENSOR_BYTES] = 1;
            if(pin == DHT2_PIN)
                dataBuffer[SENSOR_BYTES] = 2;
            pru_rpmsg_send(&transport, dst, src, dataBuffer, SENSOR_BYTES + 1);
        }
        synchronize(); // Wait until HOST is ready
    }
}
static bool start(void)
{
    unsigned attempts;
    bank->DATAOUT&= ~pin;   // Set pin output bit LO
    bank->OE&= ~pin;        // Set DHT pin direction as output
    __delay_cycles(MASTER_START_LO); // Wait 5ms for sensor
    bank->OE|= pin;        // Set DHT pin as input, floats HI for response signal
    attempts = 0;
    while((bank->DATAIN&pin) != pin)
    {   // Sensor should be LO wait for float HI
        if(attempts++ > ATTEMPTS_TO_PULLUP)
            return false;
        __delay_cycles(1*CLKFREQ);
    };
    attempts = 0;
    while((bank->DATAIN&pin) == pin)
    {   // Sensor is HI look for LO
        if(attempts++ > ATTEMPTS_TO_START)
            return false;
        __delay_cycles(1*CLKFREQ);
    };
    attempts = 0;
    while((bank->DATAIN&pin) != pin)
    {   // Sensor is HI
        if(attempts++ > ATTEMPTS_TO_START)
            return false;
       __delay_cycles(1*CLKFREQ);
    };
    attempts = 0;
    while((bank->DATAIN&pin) == pin)
    {   // Sensor is HI look for LO start of data
        if(attempts++ > ATTEMPTS_TO_START)
            return false;
        __delay_cycles(1*CLKFREQ);
    };
    return true;
}
// Read a bit. Port and bit is used to cut lookup time and provide more certain timing.
static bool readBit(void)
{   // Enters with sensor HI from start,
    unsigned  readHI_attempts = 0, readLO_attempts = 0;
    while(readLO_attempts++ < SENSOR_READ_LO)
    {   // Loops until HI start pulse
        if((bank->DATAIN&pin) == pin)
            break;
        __delay_cycles(1*CLKFREQ);
    };
    while(readHI_attempts++ < SENSOR_READ_HI)
    {   // Loops until LO end of bit
        if((bank->DATAIN&pin) != pin)
            break;
        __delay_cycles(1*CLKFREQ);
    };
    if(readHI_attempts < readLO_attempts)
        return false;
    return true;
}
// Read a byte
static uint8_t read(void)
{
    int i;
    uint8_t bit =  0x80;
    uint8_t byte = 0;
    bool state;
    for(i=0; i<8; i++)
    {   // Shift 8 bits to form byte
       state =readBit();
       if(state)
           byte|= bit;
        bit>>= 1;
    }
    return byte;
}
static void readSensor(void)
{
    int i;
    uint8_t data;
    for(i = 0 ; i < SENSOR_BYTES ; i++)
    {
        data = read();
        dataBuffer[i] = data;
    }
    __delay_cycles(100*CLKFREQ);
}

void initializeMessage(void)
{
    volatile uint8_t *status;
    /* Clear the status of the PRU-ICSS system event that the ARM will use to 'kick' us */
     CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
     /* Make sure the Linux drivers are ready for RPMsg communication */
     status = &resourceTable.rpmsg_vdev.status;
     while(!(*status & VIRTIO_CONFIG_S_DRIVER_OK));
     /* Initialize the RPMsg transport structure */
     pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);
     /* Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
     while(pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);
}
void waitForStart(void)
{
    uint16_t len = SENSOR_BYTES + 1;
    bool startFlag = true;
    while(startFlag)
    {   // Loop until we get some input from the BBB HOST ready to start
        GPIO1->DATAOUT^= DEBUG_PIN;
        __delay_cycles(500*CLKFREQ);
        if(__R31 & HOST_INT)
        {   // Event occured
            CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST; // Clear the event status
            while(pru_rpmsg_receive(&transport, &src, &dst, dataBuffer, &len) == PRU_RPMSG_SUCCESS)
            {   // Look for character 'S' to start
                if(dataBuffer[0] == 'S')
                {
                    pru_rpmsg_send(&transport, dst, src, dataBuffer, 1);
                    startFlag = false;
                    break;
                }
            }
        }
    }
}
void synchronize(void)
{
    uint16_t len = SENSOR_BYTES + 1;
    bool startFlag = true;
    while(startFlag)
    {   // Loop until we get some input from the BBB HOST ready to start
//        GPIO1->DATAOUT^= DEBUG_PIN;
//        __delay_cycles(200*CLKFREQ);
        if(__R31 & HOST_INT)
        {   // Event occured
            CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST; // Clear the event status
            while(pru_rpmsg_receive(&transport, &src, &dst, dataBuffer, &len) == PRU_RPMSG_SUCCESS)
            {   // Look for character 'S' to start
                if(dataBuffer[0] == 'S')
                {
                    startFlag = false;
                    break;
                }
            }
        }
    }
}

