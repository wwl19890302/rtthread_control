/*
 * File      : servo_motor.h
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

#ifndef __SERVO_MOTOR_H__
#define __SERVO_MOTOR_H__

#include <rtthread.h>

#define INTSPD3_Pin     GPIO_Pin_6
#define INTSPD3_PORT    GPIOC
#define INTSPD3_RCC     RCC_APB2Periph_GPIOC

#define INTSPD2_Pin     GPIO_Pin_7
#define INTSPD2_PORT    GPIOC
#define INTSPD2_RCC     RCC_APB2Periph_GPIOC

#define INTSPD1_Pin     GPIO_Pin_8
#define INTSPD1_PORT    GPIOC
#define INTSPD1_RCC     RCC_APB2Periph_GPIOC

#define SRV_ON_Pin      GPIO_Pin_9
#define SRV_ON_PORT     GPIOC
#define SRV_ON_RCC      RCC_APB2Periph_GPIOC

#define INTSPD3_OUT_ON  (GPIO_ResetBits(INTSPD3_PORT, INTSPD3_Pin))
#define INTSPD3_OUT_OFF (GPIO_SetBits(INTSPD3_PORT, INTSPD3_Pin))

#define INTSPD2_OUT_ON  (GPIO_ResetBits(INTSPD3_PORT, INTSPD3_Pin))
#define INTSPD2_OUT_OFF (GPIO_SetBits(INTSPD3_PORT, INTSPD3_Pin))

#define INTSPD1_OUT_ON  (GPIO_ResetBits(INTSPD3_PORT, INTSPD3_Pin))
#define INTSPD1_OUT_OFF (GPIO_SetBits(INTSPD3_PORT, INTSPD3_Pin))

#define SRV_ON_EN       (GPIO_ResetBits(INTSPD3_PORT, INTSPD3_Pin))
#define SRV_ON_DI       (GPIO_SetBits(INTSPD3_PORT, INTSPD3_Pin))

#define SRV_SPEED_1		INTSPD1_OUT_OFF;INTSPD2_OUT_OFF;INTSPD3_OUT_OFF;
#define SRV_SPEED_2     INTSPD1_OUT_ON; INTSPD2_OUT_OFF;INTSPD3_OUT_OFF;
#define SRV_SPEED_3     INTSPD1_OUT_OFF;INTSPD2_OUT_ON; INTSPD3_OUT_OFF;
#define SRV_SPEED_4     INTSPD1_OUT_ON; INTSPD2_OUT_ON; INTSPD3_OUT_OFF;
#define SRV_SPEED_5     INTSPD1_OUT_OFF;INTSPD2_OUT_OFF;INTSPD3_OUT_ON;
#define SRV_SPEED_6     INTSPD1_OUT_ON; INTSPD2_OUT_OFF;INTSPD3_OUT_ON;
#define SRV_SPEED_7     INTSPD1_OUT_OFF;INTSPD2_OUT_ON; INTSPD3_OUT_ON;
#define SRV_SPEED_8     INTSPD1_OUT_ON; INTSPD2_OUT_ON; INTSPD3_OUT_ON;

void SERVO_MOTOR_GPIO_Init(void);
void SERVO_MOTOR_on(uint8_t speed);
void SERVO_MOTOR_off(void);
#endif
