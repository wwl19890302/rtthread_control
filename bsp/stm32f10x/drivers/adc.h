#ifndef __ADC_H__
#define __ADC_H__

#include "rtthread.h"

void ADC_VBAT_Init(void);
rt_uint16_t Get_Adc(rt_uint8_t ch);
rt_uint16_t	Get_Adc_Average(rt_uint8_t ch,rt_uint8_t times);
int rt_adc_init(void);

#endif

