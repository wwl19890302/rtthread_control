#include "ds18b20.h"
#include "rtthread.h"

#define DS18B20_Pin     GPIO_Pin_1
#define DS18B20_PORT    GPIOB
#define DS18B20_RCC     RCC_APB2Periph_GPIOB

#define DS18B20_DQ_OUT_LOW  (GPIO_ResetBits(DS18B20_PORT, DS18B20_Pin))
#define DS18B20_DQ_OUT_HIGH (GPIO_SetBits(DS18B20_PORT, DS18B20_Pin))
#define DS18B20_DQ_IN       (GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_Pin))

#define MAXSENSORNUM    8

uint8_t DS18B20_ID[MAXSENSORNUM][8];
uint8_t DS18B20_sensornum;

static void delaynop(rt_uint32_t count)
{
//     while (count--) {
//         __NOP();
//     }
}

void DS18B20_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(DS18B20_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = DS18B20_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_Init(DS18B20_PORT, &GPIO_InitStructure);

    GPIO_SetBits(DS18B20_PORT, DS18B20_Pin);
}

void DS18B20_Mode_IPU(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DS18B20_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure_Init(DS18B20_PORT, &GPIO_InitStructure);
}

void DS18B20_Mode_Out(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DS18B20_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_Init(DS18B20_PORT, &GPIO_InitStructure);
}

void DS18B20_reset(void)
{
    DS18B20_Mode_Out();
    DS18B20_DQ_OUT_LOW;
    delaynop(480);
    DS18B20_DQ_OUT_HIGH;
    delaynop(10);
}

u8 DS18B20_answercheck(void)
{
    uint8_t delay = 0;
    DS18B20_Mode_IPU();

    while(DS18B20_DQ_IN&&delay < 100)
    {
        delay++;
        delaynop(1);
    }
    if(delay >= 100)
        return 1;
    else
        delay = 0;
    while(!DS18B20_DQ_IN &&delay < 240)
    {
        delay++;
        delaynop(1);
    }
    if(delay >= 240)
        return 1;
    return 0;
}

















#endif
