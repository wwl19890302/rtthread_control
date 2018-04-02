#include "ds1302.h"
#include "stdio.h"
#include <stm32f10x.h>

uint8_t u8time[8];
uint8_t rtc_init[8] = {
    10, //sec
    42, //min
    14, //hour
    2, //day
    4,  //month
    1,  //week
    18, //year
    0,
};

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t   ds1302_stack[ 512 ];
static struct rt_thread ds1302_thread;

/**************************************
 * void DS1302_GPIO_Init(void)
 *
 * DS1302_SDA-->OUT_PP
 *
 * ***************************/
void DS1302_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    GPIO_InitStructure.GPIO_Pin = DS1302_SCK | DS1302_CE;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DS1302_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	DS1302_write(DS1302_WRPROT, 0X80);  //OPEN write protect
}

static void delaynop(rt_uint32_t count)
{
//     while (count--) {
//         __NOP();
//     }
}

void DS1302_writebyte(uint8_t dat)
{
    uint8_t	i;
    Clr_sclk();

    delaynop(10);

    for(i=0;i<8;i++)
    {
        Clr_sclk();
        if(dat & 0x01)
            Set_sda();
        else
            Clr_sda();
        delaynop(10);
        Set_sclk();
        dat >>= 1;
    }
}

void DS1302_write(uint8_t addr, uint8_t dat)
{
    DI_ce();
    Clr_sclk();
    delaynop(10);
    EN_ce();
    delaynop(10);
    DS1302_writebyte(addr);
    DS1302_writebyte(dat);
    DI_ce();
    Clr_sclk();
    delaynop(10);
}



uint8_t DS1302_readbyte(uint8_t addr)
{
    uint8_t i;
    uint8_t dat = 0;

    DI_ce();
    Clr_sclk();
    delaynop(10);
    EN_ce();
    delaynop(10);
    DS1302_writebyte(addr);

    for(i=0;i<8;i++)
    {
        Set_sclk();
        dat >>= 1;
        Clr_sclk();
        if(Read_sda())
            dat |= 0x80;
    }
    delaynop(10);
    DI_ce();
    return dat;
}

void DS1302_settime(uint8_t time[8])
{
    uint8_t i,j,temp;
	uint8_t write[7] = {DS1302_WRSEC,DS1302_WRMIN,DS1302_WRHOUR,DS1302_WRDAY,DS1302_WRMON,DS1302_WRWEEK,DS1302_WRYEAR};

    DS1302_write(DS1302_WRPROT, 0X00);  //close write protect
	for(i=0;i<8;i++)
	{
		temp = rtc_init[i] % 10;
		rtc_init[i] = rtc_init[i]/10*16 + temp;
	}
 	for(i=0;i<7;i++)
	{
       DS1302_write(write[i],rtc_init[i]);
	}
	DS1302_write(DS1302_WRPROT, 0X80);  //OPEN write protect
	

}

void DS1302_readtime(void)
{
    uint8_t i,temp;

    u8time[0] = DS1302_readbyte(DS1302_RDSEC);
    u8time[1] = DS1302_readbyte(DS1302_RDMIN);
    u8time[2] = DS1302_readbyte(DS1302_RDHOUR);
    u8time[3] = DS1302_readbyte(DS1302_RDDAY);
    u8time[4] = DS1302_readbyte(DS1302_RDMON);
    u8time[5] = DS1302_readbyte(DS1302_RDWEEK);
    u8time[6] = DS1302_readbyte(DS1302_RDYEAR);

	for(i=0;i<7;i++)
	{
		temp = u8time[i] / 16;
		u8time[i] = temp * 10 + u8time[i]%16;
	}

    rt_kprintf("%d-%d-%d %d:%d:%dweek%d\r\n",u8time[6],u8time[4],u8time[3],u8time[2],u8time[1],u8time[0],u8time[5]);
}

static void ds1302_thread_entry(void* parameter)
{
    DS1302_GPIO_Init();
//    DS1302_settime(rtc_init);

    while(1)
    {
        rt_thread_delay(RT_TICK_PER_SECOND*1);
		DS1302_readtime();
    }
}

int rt_ds1302_init(void)
{
    rt_err_t result;

    result = rt_thread_init(&ds1302_thread,
                            "ds1302",
                            ds1302_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&ds1302_stack[0],
                            sizeof(ds1302_stack),
                            18,5);
    if(result == RT_EOK)
    {
        rt_thread_startup(&ds1302_thread);
        return 0;
    }
    rt_kprintf("ds1302 thread startup failed!");
    return -1;
}




















