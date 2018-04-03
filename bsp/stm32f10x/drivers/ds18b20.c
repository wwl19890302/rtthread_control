#include "ds18b20.h"
#include "rtthread.h"
#include "rthw.h"
#include "stdio.h"

#define DS18B20_Pin     GPIO_Pin_1
#define DS18B20_PORT    GPIOB
#define DS18B20_RCC     RCC_APB2Periph_GPIOB

#define DS18B20_DQ_OUT_LOW  (GPIO_ResetBits(DS18B20_PORT, DS18B20_Pin))
#define DS18B20_DQ_OUT_HIGH (GPIO_SetBits(DS18B20_PORT, DS18B20_Pin))
#define DS18B20_DQ_IN       (GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_Pin))

#define MAXSENSORNUM    8

uint8_t DS18B20_ID[MAXSENSORNUM][8];
uint8_t DS18B20_sensornum;

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t   ds18b20_stack[ 512 ];
static struct rt_thread ds18b20_thread;

void rt_hw_us_delay(rt_uint32_t us) //must < 1 ostick
{
    rt_uint32_t delta;

    us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));

    delta = SysTick->VAL;

    while (delta - SysTick->VAL < us);
}

void DS18B20_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(DS18B20_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = DS18B20_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);

    GPIO_SetBits(DS18B20_PORT, DS18B20_Pin);
}

void DS18B20_Mode_IPU(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DS18B20_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

void DS18B20_Mode_Out(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DS18B20_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

void DS18B20_reset(void)
{
    DS18B20_Mode_Out();
    DS18B20_DQ_OUT_LOW;
    rt_hw_us_delay(750);
    DS18B20_DQ_OUT_HIGH;
    rt_hw_us_delay(15);
}

u8 DS18B20_answercheck(void)
{
    uint8_t delay = 0;
    DS18B20_Mode_IPU();

    while(DS18B20_DQ_IN&&delay < 200)
    {
        delay++;
        rt_hw_us_delay(1);
    }
    if(delay >= 200)
        return 1;
    else
        delay = 0;
    while(!DS18B20_DQ_IN &&delay < 240)
    {
        delay++;
        rt_hw_us_delay(1);
    }
    if(delay >= 240)
        return 1;
    return 0;
}

uint8_t DS18B20_readbit(void)
{
    u8 data;
    DS18B20_Mode_Out();
    DS18B20_DQ_OUT_LOW;
    rt_hw_us_delay(2);
    DS18B20_DQ_OUT_HIGH;
	DS18B20_Mode_IPU();
    rt_hw_us_delay(12);
    
    if(DS18B20_DQ_IN)
        data = 1;
    else
        data = 0;
    rt_hw_us_delay(50);
    return data;
}

uint8_t DS18B20_read2bit(void)
{
    uint8_t i;
    uint8_t dat = 0;
    for(i=0;i<2;i++)
    {
        dat = dat<<1;
        DS18B20_Mode_Out();
        DS18B20_DQ_OUT_LOW;
        rt_hw_us_delay(2);
        DS18B20_DQ_OUT_HIGH;
        DS18B20_Mode_IPU();
        rt_hw_us_delay(12);
        if(DS18B20_DQ_IN)
            dat |= 0x01;
        rt_hw_us_delay(50);
    }
    return dat;
}

uint8_t DS18B20_readbyte(void)
{
    uint8_t i,j,dat = 0;
    for(i=0;i<8;i++)
    {
        j = DS18B20_readbit();
        dat = dat | (j<<i);
    }
    return dat;
}

void DS18B20_writebit(u8 dat)
{
    DS18B20_Mode_Out();
    if(dat)                 //write 1
    {
        DS18B20_DQ_OUT_LOW;
        rt_hw_us_delay(2);
        DS18B20_DQ_OUT_HIGH;
        rt_hw_us_delay(60);
    }
    else                    //write 0
    {
        DS18B20_DQ_OUT_LOW;
        rt_hw_us_delay(60);
        DS18B20_DQ_OUT_HIGH;
        rt_hw_us_delay(2);
    }
}

void DS18B20_writebyte(uint8_t dat)
{
    uint8_t i,testb;
    DS18B20_Mode_Out();
    for(i=0;i<8;i++)
    {
        testb = dat & 0x01;
        dat = dat>>1;
        if(testb)               //write 1
        {
            DS18B20_DQ_OUT_LOW;
            rt_hw_us_delay(2);
            DS18B20_DQ_OUT_HIGH;
            rt_hw_us_delay(60);
        }
        else                    //write 0
        {
            DS18B20_DQ_OUT_LOW;
            rt_hw_us_delay(60);
            DS18B20_DQ_OUT_HIGH;
            rt_hw_us_delay(2);
        }
    }
}

uint8_t DS18B20_Init(void)
{
    DS18B20_GPIO_Init();
    DS18B20_reset();
    return DS18B20_answercheck();
}

float DS18B20_gettemp(uint8_t i)
{
    uint8_t j,TL,TH;
    short temperature;
    float temperature1;
    DS18B20_reset();
    DS18B20_answercheck();
    DS18B20_writebyte(0xcc);
    DS18B20_writebyte(0x44);
    DS18B20_reset();
    DS18B20_answercheck();

//     DS18B20_writebyte(0x55);
//     for(j=0;j<8;j++)
//     {
//         DS18B20_writebyte(DS18B20_ID[i][j]);
//     }
	DS18B20_writebyte(0xcc);
    DS18B20_writebyte(0xbe);
    TL = DS18B20_readbyte();    //LSB
    TH = DS18B20_readbyte();    //MSB
//	rt_kprintf("%2x %2x\r\n",TH,TL);
    if(TH > 7)
    {
        temperature = (TH<<8) + TL;
        temperature1 = (~temperature);
        temperature1 *= (-0.0625);
    }
    else
    {
        temperature1 = (float)((TH<<8) + TL) * 0.0625;
    }
    return  temperature1;
}

void DS18B20_Searchrom(void)
{
    uint8_t k,l,chongtuwei,m,n,num,s;
    uint8_t zhan[5];
    uint8_t ss[64];
    uint8_t temp;
    l = 0;num = 0;s=0;
    do
    {
        DS18B20_reset();
        rt_hw_us_delay(480);
        DS18B20_writebyte(0XF0);
        for(m=0;m<8;m++)
        {
            s = 0;
            for(n = 0;n<8;n++)
            {
                k = DS18B20_read2bit();

                k = k & 0x03;
                s >>= 1;
                if(k == 0x01)
                {
                    DS18B20_writebit(0);
                    ss[(m*8+n)] = 0;
                }
                else if(k == 0x02)
                {
                    s = s|0x80;
                    DS18B20_writebit(1);
                    ss[(m*8+n)] = 1;
                }
                else if(k == 0x00)
                {
                    chongtuwei = m*8+n+1;
                    if(chongtuwei > zhan[l])
                    {
                        DS18B20_writebit(0);
                        ss[(m*8+n)] = 0;
                        zhan[++l] = chongtuwei;
                    }
                    else if(chongtuwei < zhan[l])
                    {
                        s = s | ((ss[(m*8+n)] & 0x01) << 7);
                        DS18B20_writebit(ss[(m*8+n)]);
                    }
                    else if(chongtuwei == zhan[l])
                    {
                        s = s | 0x80;
                        DS18B20_writebit(1);
                        ss[(m*8+n)] = 1;
                        l = l - 1;
                    }
                }
                else
                {
                    //search fail
                }
            }
            temp = s;
            DS18B20_ID[num][m] = temp;
        }
        num = num +1;
    }while((zhan[1] != 0) && (num < MAXSENSORNUM));
    DS18B20_sensornum = num;
}

static void ds18b20_thread_entry(void* parameter)
{
    uint8_t i = 0;
	uint8_t s1[5],s2[5];
    float temperature;
    while((DS18B20_Init()) && (i<5))
    {
        rt_kprintf("DS18B20 check faild!\r\n");
        i++;
    }
    if(i<5)
        rt_kprintf("DS18B20 check ready!\r\n");

//    DS18B20_Searchrom();

    while(1)
    {
        temperature = DS18B20_gettemp(0);
        sprintf(s1,"%.2f",temperature);
// 		temperature = DS18B20_gettemp(1);
//         sprintf(s2,"%.2f",temperature);
        rt_kprintf("ID:%02x%02x%02x%02x%02x%02x%02x%02x temp:%s\r\n",DS18B20_ID[0][0],
                DS18B20_ID[0][1],DS18B20_ID[0][2],DS18B20_ID[0][3],DS18B20_ID[0][4],
                DS18B20_ID[0][5],DS18B20_ID[0][6],DS18B20_ID[0][7],s1);
// 		rt_kprintf("ID:%02x%02x%02x%02x%02x%02x%02x%02x temp:%s\r\n",DS18B20_ID[1][0],
//                 DS18B20_ID[1][1],DS18B20_ID[1][2],DS18B20_ID[1][3],DS18B20_ID[1][4],
//                 DS18B20_ID[1][5],DS18B20_ID[1][6],DS18B20_ID[1][7],s2);
        rt_thread_delay(RT_TICK_PER_SECOND*3);
    }
}

int rt_ds18b20_init(void)
{
    rt_err_t result;
    result = rt_thread_init(&ds18b20_thread,
                            "ds18b20",
                            ds18b20_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&ds18b20_stack[0],
                            sizeof(ds18b20_stack),
                            17,15);
    if(result == RT_EOK)
    {
        rt_thread_startup(&ds18b20_thread);
        return 0;
    }
    rt_kprintf("ds18b20 thread startup failed!");
    return -1;
}





