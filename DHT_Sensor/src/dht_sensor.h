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

#ifndef DHT_SENSOR_H_
#define DHT_SENSOR_H_


volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* The PRU-ICSS system events used for RPMsg are defined in the Linux device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU1 uses system event 18 (To ARM) and 19 (From ARM)
 * Host-0 Interrupt sets bit 30 in register R31 */
#define HOST_INT            ((uint32_t) 1 << 30)
#define TO_ARM_HOST         16
#define FROM_ARM_HOST       17

/* * Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 * at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c */
#define CHAN_NAME                   "rpmsg-pru"
#define CHAN_DESC                   "Channel 30"
#define CHAN_PORT                   30
#define VIRTIO_CONFIG_S_DRIVER_OK   4

#define CLKFREQ                    200u           // 200 Mhz PRU Clock frequency
#define MASTER_START_LO           (5000*CLKFREQ) // master drives low for: 1 < t < 10ms
#define MASTER_PULLUP_DELAY       (40*CLKFREQ)   // master pulls high: 20us < t > 40us
#define SENSOR_PULLS_LO           (80*CLKFREQ)   // master waits for sensor to pull LO: t < 80us
#define SENSOR_PULLS_HI           (80*CLKFREQ)   // master waits for sensor to pull HI: t < 80us

// PRU Peripheral memory locations, and IO pin definitions
#define GPIO0_OFFSET              0x44E07000
#define GPIO1_OFFSET              0x4804C000
#define GPIO2_OFFSET              0x481AC000
#define GPIO3_OFFSET              0x481AE000

// P8 PRU0 pins
#define GPIO1_P8_11_BIT13        0x00002000
#define GPIO1_P8_12_BIT12        0x00001000
#define GPIO1_P8_15_BIT15        0x00008000
#define GPIO1_P8_16_BIT14        0x00004000

#define R30_P8_11_BIT15          0x00008000
#define R30_P8_12_BIT14          0x00004000
#define R31_P8_15_BIT15          0x00008000
#define P31_P8_16_BIT14          0x00004000

//P9 PRU0 pins
#define GPIO3_P9_24_BIT15       0x00008000
#define GPIO3_P9_25_BIT21       0x00200000
#define GPIO3_P9_27_BIT19       0x00080000
#define GPIO3_P9_28_BIT17       0x00010000
#define GPIO3_P9_29_BIT15       0x00008000
#define GPIO3_P9_30_BIT16       0x00010000
#define GPIO3_P9_31_BIT14       0x00004000

#define R31_P9_25_BIT7          0x00000080
#define R31_P9_27_BIT5          0x00000020
#define R31_P9_28_BIT3          0x00000008
#define R31_P9_29_BIT1          0x00000002
#define R31_P9_30_BIT2          0x00000004
#define R31_P9_31_BIT0          0x00000001

// PRU Shared memory locations, and index offsets
#define PRU_SHARED_RAM          0x00000000
#define SENSOR_BYTES            5

// Number of read sensor input counter loop approximately 1us for delay
#define ATTEMPTS_TO_START         100
#define ATTEMPTS_TO_PULLUP        100
#define SENSOR_READ_LO            70           // Count to HI max
#define SENSOR_READ_HI            70           // Count to LO max


#endif /* DHT_SENSOR_H_ */
