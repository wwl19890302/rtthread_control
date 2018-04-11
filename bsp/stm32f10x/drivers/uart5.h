#ifndef __UART5_H__
#define __UART5_H__

#include <rthw.h>
#include "stm32f10x.h"

#define UART5_TX_PIN    GPIO_Pin_12
#define UART5_TX_PORT   GPIOC
#define UART5_TX_RCC    RCC_APB2Periph_GPIOC

#define UART5_RX_PIN    GPIO_Pin_2
#define UART5_RX_PORT   GPIOD
#define UART5_RX_RCC    RCC_APB2Periph_GPIOD

void usr_echo_init(void);

#endif
