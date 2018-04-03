#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "stm32f10x.h"

uint8_t DS18B20_Init(void);
uint8_t DS18B20_readbyte(void);
uint8_t DS18B20_readbit(void);
uint8_t DS18B20_answercheck(void);
void    DS18B20_GPIO_Init(void);
void    DS18B20_Mode_IPU(void);
void    DS18B20_Mode_Out(void);
void    DS18B20_reset(void);
void    DS18B20_Searchrom(void);
void    DS18B20_writebyte(uint8_t dat);
float   DS18B20_gettemp(uint8_t i);
int     rt_ds18b20_init(void);

#endif
