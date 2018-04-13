#include "spi_data.h"
#include "stm32f10x.h"
#include "rtthread.h"

//#define SPI_DATA_DEVICE_SPI1
#define SPI_DATA_DEVICE_SPI2
#define SPI_DATA_SPI_MODE   SPI_Mode_Master
//#define SPI_DATA_SPI_MODE   SPI_Mode_Slave

#ifdef SPI_DATA_DEVICE_SPI1
#define spi_data_gpio_rcc       (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO)
#define spi_data_gpio           GPIOA
#define spi_data_pin            (GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7)
#endif

#ifdef SPI_DATA_DEVICE_SPI2
#define spi_data_gpio_rcc       (RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO)
#define spi_data_gpio           GPIOB
#define spi_data_pin            (GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)
#endif

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t spi_data_stack[ 512 ];
static struct rt_thread spi_data_thread;

void SPI_DATA_GPIO_Init(void)
{
    SPI_TypeDef * spi;
    IRQn_Type irqn;
    GPIO_InitTypeDef    GPIO_InitStructure;
    SPI_InitTypeDef     SPI_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;

#ifdef SPI_DATA_DEVICE_SPI1
    spi = SPI1;
    irqn = SPI1_IRQn;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
#endif

#ifdef SPI_DATA_DEVICE_SPI2
    spi = SPI2;
    irqn = SPI2_IRQn;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
#endif
#include"stm32f10x_spi.h"
    RCC_APB2PeriphClockCmd(spi_data_gpio_rcc, ENABLE);

    GPIO_InitStructure.GPIO_Pin = spi_data_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(spi_data_gpio, &GPIO_InitStructure);

    GPIO_SetBits(spi_data_gpio, spi_data_pin);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_DATA_SPI_MODE;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(spi, &SPI_InitStructure);

//    if(SPI_DATA_SPI_MODE == SPI_Mode_Slave)
    {
        SPI_I2S_ITConfig(spi, SPI_I2S_IT_RXNE, ENABLE);
        /* Enable the USART1 Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = irqn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }

    SPI_Cmd(spi, ENABLE);
}

void SPI_DATA_set_speed(uint8_t SPI_BaudRatePrescaler)
{
    SPI_TypeDef * spi;

#ifdef SPI_DATA_DEVICE_SPI1
    spi = SPI1;
    SPI_BaudRatePrescaler *= 2;
#endif
#ifdef SPI_DATA_DEVICE_SPI2
    spi = SPI2;
#endif
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    spi->CR1 &= 0xFFC7;
    spi->CR1 |= SPI_BaudRatePrescaler;
    SPI_Cmd(spi, ENABLE);
}

void SPI_DATA_write_byte(uint8_t dat)
{
    SPI_TypeDef * spi;
    uint8_t retry = 0;
#ifdef SPI_DATA_DEVICE_SPI1
    spi = SPI1;
#endif
#ifdef SPI_DATA_DEVICE_SPI2
    spi = SPI2;
#endif
    while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET)
    {
        retry++;
        if(retry > 100)
            return;
    }
    SPI_I2S_SendData(spi, dat);
    retry = 0;
}
#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT_ALIAS(SPI_DATA_write_byte, spi_data_write, spi_data send one byte.eg:spi_data_write(31))
#endif

uint8_t SPI_DATA_read_byte(void)
{
    SPI_TypeDef * spi;
    uint8_t retry = 0;
#ifdef SPI_DATA_DEVICE_SPI1
    spi = SPI1;
#endif
#ifdef SPI_DATA_DEVICE_SPI2
    spi = SPI2;
#endif
    while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET)
    {
        retry++;
        if(retry > 100)
            return 0;
    }
	retry = 0;
    return SPI_I2S_ReceiveData(spi); 
}
#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT_ALIAS(SPI_DATA_read_byte, spi_data_read, spi_data receive one byte.eg:spi_data_read())
#endif

#ifdef SPI_DATA_DEVICE_SPI1
void SPI1_IRQHandler(void)
#endif
#ifdef SPI_DATA_DEVICE_SPI2
void SPI2_IRQHandler(void)
#endif
{
	SPI_TypeDef * spi;
    uint8_t dat;
#ifdef SPI_DATA_DEVICE_SPI1
    spi = SPI1;
#endif
#ifdef SPI_DATA_DEVICE_SPI2
    spi = SPI2;
#endif
    if(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == SET)
    {
        dat = SPI_I2S_ReceiveData(spi);
//        SPI_DATA_write_byte(dat);
		rt_kprintf("%d\r\n",dat);
    }
}

static void spi_data_thread_entry(void* parameter)
{
    rt_uint8_t ch,i;
    SPI_DATA_GPIO_Init();
    SPI_DATA_set_speed(SPI_BaudRatePrescaler_8);

    while(1)
    {
//         SPI_DATA_write_byte(i++);
//         ch = SPI_DATA_read_byte();
//         rt_kprintf("%d %d\r\n",i,ch);
        rt_thread_delay(RT_TICK_PER_SECOND *1);
    }
}

int rt_spi_data_init(void)
{
    rt_err_t result;

    /* init spi_data thread */
    result = rt_thread_init(&spi_data_thread,
                            "spi_data",
                            spi_data_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&spi_data_stack[0],
                             sizeof(spi_data_stack),
                             20,5);

    if(result == RT_EOK)
    {
        rt_thread_startup(&spi_data_thread);
        return 0;
    }
    rt_kprintf("spi_data thread startup failed!");
    return -1;
}



