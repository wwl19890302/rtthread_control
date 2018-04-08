#include "servo_motor.h"
#include "stm32f10x.h"

void SERVO_MOTOR_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(INTSPD1_RCC | INTSPD2_RCC | INTSPD3_RCC | SRV_ON_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin = INTSPD1_Pin;
    GPIO_Init(INTSPD1_PORT, &GPIO_InitStructure);
    INTSPD1_OUT_OFF;

    GPIO_InitStructure.GPIO_Pin = INTSPD2_Pin;
    GPIO_Init(INTSPD2_PORT, &GPIO_InitStructure);
    INTSPD2_OUT_OFF;

    GPIO_InitStructure.GPIO_Pin = INTSPD3_Pin;
    GPIO_Init(INTSPD3_PORT, &GPIO_InitStructure);
    INTSPD3_OUT_OFF;

    GPIO_InitStructure.GPIO_Pin = SRV_ON_Pin;
    GPIO_Init(SRV_ON_PORT, &GPIO_InitStructure);
    SRV_ON_DI;
}

void SERVO_MOTOR_on(uint8_t speed)
{
    switch (speed) {
        case 1:
            SRV_SPEED_1;
            break;
        case 2:
            SRV_SPEED_2;
            break;
        case 3:
            SRV_SPEED_3;
            break;
        case 4:
            SRV_SPEED_4;
            break;
        case 5:
            SRV_SPEED_5;
            break;
        case 6:
            SRV_SPEED_6;
            break;
        case 7:
            SRV_SPEED_7;
            break;
        case 8:
            SRV_SPEED_8;
            break;
        default:
            break;
    }
    rt_thread_delay(RT_TICK_PER_SECOND/100);
    SRV_ON_EN;
}

void SERVO_MOTOR_off(void)
{
    SRV_ON_DI;
    rt_thread_delay(RT_TICK_PER_SECOND);
}
