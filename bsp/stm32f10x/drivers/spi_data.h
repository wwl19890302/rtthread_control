/*
 * File      : spi_data.h
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
 */

#ifndef __SPI_DATA_H__
#define __SPI_DATA_H__

#include <rtthread.h>

uint8_t SPI_DATA_read_byte(void);
void SPI_DATA_write_byte(uint8_t dat);
int rt_spi_data_init(void);

#endif
