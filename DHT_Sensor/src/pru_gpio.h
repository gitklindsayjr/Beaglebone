/*
 * Copyright (C) 2016-2018 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
typedef struct{
  union {
    volatile unsigned REVISION;
    volatile struct {
      unsigned MINOR : 6;
      unsigned CUSTOM: 2;
      unsigned MAJOR: 3;
      unsigned RTL: 5;
      unsigned FUNC: 12;
      unsigned rsrvd: 2;
      unsigned SCHEME: 2;
    } REVISION_bit;
  } ; //0h - 3h, reset: 50600801h
  unsigned rsrvd0[3]; //4h-fh
  union{
    volatile unsigned SYSCONFIG;
    volatile struct{
      unsigned AUTOIDLE: 1;
      unsigned SOFTRESET: 1;
      unsigned ENAWAKEUP: 1;
      unsigned IDLEMODE: 2;
      unsigned rsrvd: 27;
    } SYSCONFIG_bit;
  } ;//10h-13h, reset: 0h
  unsigned rsrvd1[3]; //13h-1fh
  union{
    volatile unsigned EOI; 
    volatile struct{
      unsigned DMAEVENT_Ack: 1;
      unsigned rsrvd: 31;
    } EOI_bit;
  } ;//20h-23h, reset: 0h
  volatile unsigned IRQSTATUS_RAW_0; //24h-27h, reset: 0h
  volatile unsigned IRQSTATUS_RAW_1; //28h-2Bh, reset: 0h
  volatile unsigned IRQSTATUS_0; //2Ch-2Fh, reset: 0h
  volatile unsigned IRQSTATUS_1; //30h-33h, reset: 0h
  volatile unsigned IRQSTATUS_SET_0; //34h-37h, reset: 0h
  volatile unsigned IRQSTATUS_SET_1; //38h-3Bh, reset: 0h
  volatile unsigned IRQSTATUS_CLR_0; //3Ch-3Fh, reset: 0h
  volatile unsigned IRQSTATUS_CLR_1; //40h-43h, reset: 0h
  volatile unsigned IRQWAKEN_0; //44h-47h, reset: 0h
  volatile unsigned IRQWAKEN_1; //48h-4Bh, reset: 0h
  unsigned rsrvd2[0x32]; //4Ch-113h
  union{
    volatile unsigned SYSSTATUS;
    volatile struct{
      unsigned RESETDONE: 1;
      unsigned rsrvd: 31;
    } SYSSTATUS_bit;
  } ; //114h-117h
  unsigned rsrvd3[0x6]; //118h-12fh
  union{
    volatile unsigned CTRL;
    volatile struct {
      unsigned DISABLEMODULE: 1;
      unsigned GATINGRATIO: 2;
      unsigned rsrvd: 29;
    } CTRL_bit;
  } ; //130h-133h

  union{
    volatile unsigned OE;
    volatile struct{
      unsigned OE_bit0: 1; // 0
      unsigned OE_bit1: 1; // 1
      unsigned OE_bit2: 1; // 2
      unsigned OE_bit3: 1; // 3

      unsigned OE_bit4: 1; // 2
      unsigned OE_bit5: 1; // 5
      unsigned OE_bit6: 1; // 6
      unsigned OE_bit7: 1; // 7

      unsigned OE_bit8: 1;
      unsigned OE_bit9: 1;
      unsigned OE_bita: 1; // 10
      unsigned OE_bitb: 1; // 11

      unsigned OE_bitc: 1; // 12
      unsigned OE_bitd: 1; // 13
      unsigned OE_bite: 1; // 14
      unsigned OE_bitf: 1; // 15

      unsigned OE_bit10: 1;
      unsigned OE_bit11: 1;
      unsigned OE_bit12: 1;
      unsigned OE_bit13: 1;	  

      unsigned OE_bit14: 1;
      unsigned OE_bit15: 1;
      unsigned OE_bit16: 1;
      unsigned OE_bit17: 1;

      unsigned OE_bit18: 1;
      unsigned OE_bit19: 1;
      unsigned OE_bit1a: 1;
      unsigned OE_bit1b: 1;

      unsigned OE_bit1c: 1;
      unsigned OE_bit1d: 1;
      unsigned OE_bit1e: 1;
      unsigned OE_bit1f: 1;
    } OE_bit;
  }; //134h-137h, reset: ffffffffh	
  union{
    volatile unsigned DATAIN;
    volatile struct{
      unsigned DI_bit0: 1;
      unsigned DI_bit1: 1;
      unsigned DI_bit2: 1;
      unsigned DI_bit3: 1;

      unsigned DI_bit4: 1;
      unsigned DI_bit5: 1;
      unsigned DI_bit6: 1;
      unsigned DI_bit7: 1;
      
      unsigned DI_bit8: 1;
      unsigned DI_bit9: 1;
      unsigned DI_bita: 1;
      unsigned DI_bitb: 1;
      
      unsigned DI_bitc: 1;
      unsigned DI_bitd: 1;
      unsigned DI_bite: 1;
      unsigned DI_bitf: 1;
    } DATAIN_bit;
  };//138h-13Bh, reset: 0h
  volatile unsigned DATAOUT; //13Ch-13Fh, reset: 0h
  volatile unsigned LEVELDETECT0; //140h, reset: 0h
  volatile unsigned LEVELDETECT1; //144h, reset: 0h
  volatile unsigned RISINGDETECT; //148h-14Bh, reset: 0h
  volatile unsigned FALLINGDETECT; //14ch-14fh, reset: 0h
  volatile unsigned DEBOUNCENABLE; //150h-153h, reset: 0h
	union{
		volatile unsigned DEBOUNCINGTIME; 
		volatile struct{
			unsigned DEBOUNCETIME: 8;
			unsigned rsrvd: 24;
		} DEBOUNCINGTIME_bit; 
	} ;//154h-157h
	unsigned rsrvd4[0xE];//158h-18fh
	union{
		volatile unsigned CLEARDATAOUT;
		volatile struct{
			unsigned CD_bit0: 1;
			unsigned CD_bit1: 1;
			unsigned CD_bit2: 1;
			unsigned CD_bit3: 1;

			unsigned CD_bit4: 1;
			unsigned CD_bit5: 1;
			unsigned CD_bit6: 1;
			unsigned CD_bit7: 1;

			unsigned CD_bit8: 1;
			unsigned CD_bit9: 1;
			unsigned CD_bita: 1;
			unsigned CD_bitb: 1;

			unsigned CD_bitc: 1;
			unsigned CD_bitd: 1;
			unsigned CD_bite: 1;
			unsigned CD_bitf: 1;

			unsigned CD_bit10: 1;
			unsigned CD_bit11: 1;
			unsigned CD_bit12: 1;
			unsigned CD_bit13: 1;

			unsigned CD_bit14: 1;
			unsigned CD_bit15: 1;
			unsigned CD_bit16: 1;
			unsigned CD_bit17: 1;

			unsigned CD_bit18: 1;
			unsigned CD_bit19: 1;
			unsigned CD_bit1a: 1;
			unsigned CD_bit1b: 1;

			unsigned CD_bit1c: 1;
			unsigned CD_bit1d: 1;
			unsigned CD_bit1e: 1;
			unsigned CD_bit1f: 1;
		} CD_bit;
	}; //190h-193h	

	union{
		volatile unsigned SETDATAOUT;
		volatile struct{
			unsigned SD_bit0: 1;
			unsigned SD_bit1: 1;
			unsigned SD_bit2: 1;
			unsigned SD_bit3: 1;

			unsigned SD_bit4: 1;
			unsigned SD_bit5: 1;
			unsigned SD_bit6: 1;
			unsigned SD_bit7: 1;

			unsigned SD_bit8: 1;
			unsigned SD_bit9: 1;
			unsigned SD_bita: 1;
			unsigned SD_bitb: 1;

			unsigned SD_bitc: 1;
			unsigned SD_bitd: 1;
			unsigned SD_bite: 1;
			unsigned SD_bitf: 1;

			unsigned SD_bit10: 1;
			unsigned SD_bit11: 1;
			unsigned SD_bit12: 1;
			unsigned SD_bit13: 1;

			unsigned SD_bit14: 1;
			unsigned SD_bit15: 1;
			unsigned SD_bit16: 1;
			unsigned SD_bit17: 1;

			unsigned SD_bit18: 1;
			unsigned SD_bit19: 1;
			unsigned SD_bit1a: 1;
			unsigned SD_bit1b: 1;

			unsigned SD_bit1c: 1;
			unsigned SD_bit1d: 1;
			unsigned SD_bit1e: 1;
			unsigned SD_bit1f: 1;
		} SD_bit;
	}; //194h-197h
} pruGPIO; //size = 198h
