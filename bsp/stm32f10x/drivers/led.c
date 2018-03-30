/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */
#include <rtthread.h>
#include <stm32f10x.h>
#include "drivers/pin.h"

// led define
#ifdef STM32_SIMULATOR
#define led1_rcc                    RCC_APB2Periph_GPIOA
#define led1_gpio                   GPIOA
#define led1_pin                    (GPIO_Pin_5)

#define led2_rcc                    RCC_APB2Periph_GPIOA
#define led2_gpio                   GPIOA
#define led2_pin                    (GPIO_Pin_6)

#else

#define led1_rcc                    RCC_APB2Periph_GPIOB
#define led1_gpio                   GPIOB
#define led1_pin                    (GPIO_Pin_8)

#endif // led define #ifdef STM32_SIMULATOR

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 512 ];
static struct rt_thread led_thread;

void rt_hw_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(led1_rcc,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = led1_pin;
    GPIO_Init(led1_gpio, &GPIO_InitStructure);
    GPIO_SetBits(led1_gpio, led1_pin);
}

void rt_hw_led_on(rt_uint32_t n)
{
    switch (n)
    {
    case 0:
        GPIO_ResetBits(led1_gpio, led1_pin);
        break;
    default:
        break;
    }
}

void rt_hw_led_off(rt_uint32_t n)
{
    switch (n)
    {
    case 0:
        GPIO_SetBits(led1_gpio, led1_pin);
        break;
    default:
        break;
    }
}

static void led_thread_entry(void* parameter)
{
    rt_uint8_t led1 = 61;
    rt_pin_mode(led1,PIN_MODE_OUTPUT);
 //   rt_hw_led_init();

    while(1)
    {
//        /* led1 toggle */
//        rt_thread_delay(RT_TICK_PER_SECOND / 2);
//        rt_hw_led_on(0);
//        rt_thread_delay(RT_TICK_PER_SECOND / 2);
//        rt_hw_led_off(0);
        rt_pin_write(led1,PIN_LOW);
        rt_thread_delay(RT_TICK_PER_SECOND / 2);
        rt_pin_write(led1,PIN_HIGH);
        rt_thread_delay(RT_TICK_PER_SECOND / 2);
    }
}

int rt_led_init(void)
{
   rt_err_t result;

   /* init led thread */
   result = rt_thread_init(&led_thread,
                           "led",
                           led_thread_entry,
                           RT_NULL,
                           (rt_uint8_t*)&led_stack[0],
                            sizeof(led_stack),
                            20,2);

   if(result == RT_EOK)
   {
       rt_thread_startup(&led_thread);
       return 0;
   }
   rt_kprintf("led thread startup failed!");
   return -1;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
static rt_uint8_t led_inited = 0;
void led(rt_uint32_t led, rt_uint32_t value)
{
    /* init led configuration if it's not inited. */
    if (!led_inited)
    {
        rt_hw_led_init();
        led_inited = 1;
    }

    if ( led == 0 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(0);
            break;
        case 1:
            rt_hw_led_on(0);
            break;
        default:
            break;
        }
    }

    if ( led == 1 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(1);
            break;
        case 1:
            rt_hw_led_on(1);
            break;
        default:
            break;
        }
    }
}
FINSH_FUNCTION_EXPORT(led, set led[0 - 1] on[1] or off[0].)
#endif

