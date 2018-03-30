/*
 * File      : ds1302.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     mgcheng      the first version
 *
 * LSB  ,rising to write, falling to read
 */

#ifndef __DS1302_H__
#define __DS1302_H__

#include<rtthread.h>

/* RAG ADDR*/
#define DS1302_RDSEC        0X81
#define DS1302_RDMIN        0X83
#define DS1302_RDHOUR       0X85
#define DS1302_RDDAY        0X87
#define DS1302_RDMON        0X89
#define DS1302_RDWEEK       0X8B
#define DS1302_RDYEAR       0X8D
#define DS1302_RDPROT       0X8F    //READ PROTECT
#define DS1302_RDTCHARG     0X91
#define DS1302_RDCLOCKBURST 0XBF

#define DS1302_WRSEC        0X80
#define DS1302_WRMIN        0X82
#define DS1302_WRHOUR       0X84
#define DS1302_WRDAY        0X86
#define DS1302_WRMON        0X88
#define DS1302_WRWEEK       0X8A
#define DS1302_WRYEAR       0X8C
#define DS1302_WRPROT       0X8E    //WRITE PROTECT
#define DS1302_WRTCHARG     0X90

/* MODE */
#define DS1302_RDCLKBURST   0XBF
#define DS1302_WRCLKBURST   0XBE
#define DS1302_RDRAMBURST   0XFF
#define DS1302_WRRAMBURST   0XFE


typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
}TIME_TypeDef;














#endif













