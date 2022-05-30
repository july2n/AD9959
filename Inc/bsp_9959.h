#include "main.h"

#ifndef __BSP_9959_H
#define __BSP_9959_H

#define CFR_ADDR 0x03
#define CSR_ADDR 0x00
#define FR1_ADDR 0x01
#define CFTWO_ADDR 0x04
#define CPOWO_ADDR 0x05
#define ACR_ADDR 0x06

#define MultiplierEnable 0x001000  // 0 means bypass the amplitude multiplier

void WriteData_9959(uint8_t addr,uint8_t num,uint8_t *data);
void IO_reset(void);
void IO_update(void);
void Write_frequence(uint8_t channel,uint32_t freq);
void Write_amplitude(uint8_t channel,uint16_t	ampli);
void Write_phase(uint8_t channel,uint16_t phase);
	
#endif
