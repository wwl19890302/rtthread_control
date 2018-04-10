#include <rtthread.h>
#include <board.h>

#ifdef RT_USING_LWIP
#include "stm32_eth.h"
#endif /* RT_USING_LWIP */

#ifdef RT_USING_SPI
#include "rt_stm32f10x_spi.h"
#include "spi_flash_w25qxx.h"

#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
#include "msd.h"
#endif /* RT_USING_DFS */

/*
 * SPI1_MOSI: PA7
 * SPI1_MISO: PA6
 * SPI1_SCK : PA5
 *
 * CS0: PA4  SPI FLASH.
 * CS1: PA2  SD card.
 * SDPOWER:PA0
 * SDDETECT:PA1
*/
#define FLASH_CS_Pin    GPIO_Pin_4
#define FLASH_CS_PORT   GPIOA
#define FLASH_CS_RCC    RCC_APB2Periph_GPIOA

#define SD_CS_Pin       GPIO_Pin_2
#define SD_CS_PORT      GPIOA
#define SD_CS_RCC       RCC_APB2Periph_GPIOA

#define SD_POWER_Pin    GPIO_Pin_0
#define SD_POWER_PORT   GPIOA
#define SD_POWER_RCC    RCC_APB2Periph_GPIOA

#define SD_DETECT_Pin   GPIO_Pin_1
#define SD_DETECT_PORT  GPIOA
#define SD_DETECT_RCC   RCC_APB2Periph_GPIOA

#define SD_DETECT_IN    (GPIO_ReadInputDataBit(SD_DETECT_PORT, SD_DETECT_Pin))

static int rt_hw_spi_init(void)
{
#ifdef RT_USING_SPI1
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Enable GPIO clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,
        ENABLE);

        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        stm32_spi_register(SPI1, &stm32_spi, "spi1");
    }

    /* attach SPI FLASH cs */
#ifdef RT_USING_SPI_FLASH
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

        /* spi10: PA4 */
        spi_cs.GPIOx = FLASH_CS_PORT;
        spi_cs.GPIO_Pin = FLASH_CS_Pin;
        RCC_APB2PeriphClockCmd(FLASH_CS_RCC, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi10", "spi1", (void*)&spi_cs);
    }
#endif

    /* attach SD CARD cs */
#ifdef RT_USING_SD_CARD
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

        /* spi11: PA2 */
        spi_cs.GPIOx = SD_CS_PORT;
        spi_cs.GPIO_Pin = SD_CS_Pin;
        RCC_APB2PeriphClockCmd(SD_CS_RCC, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi11", "spi1", (void*)&spi_cs);
    }
#endif
#endif /* RT_USING_SPI1 */

	return 0;
}
INIT_DEVICE_EXPORT(rt_hw_spi_init);
#endif /* RT_USING_SPI */

void SPI_SD_to_PC(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;   
    GPIO_SetBits(SD_POWER_PORT, SD_POWER_Pin);

             
    GPIO_InitStructure.GPIO_Pin = (SD_CS_Pin|GPIO_Pin_5 |GPIO_Pin_6| GPIO_Pin_7);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void SPI_PC_to_SD(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;   

	GPIO_InitStructure.GPIO_Pin   = SD_POWER_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SD_POWER_PORT, &GPIO_InitStructure);
	/* SD card power on. */
	GPIO_SetBits(SD_POWER_PORT, SD_POWER_Pin);
	
	rt_hw_spi_init();
}

void rt_spi_flash_device_init(void)
{
#ifdef RT_USING_SPI
//    rt_hw_spi_init();


    GPIO_InitTypeDef  GPIO_InitStructure;

	/* PA0 : SD Power */
	RCC_APB2PeriphClockCmd(SD_POWER_RCC | SD_DETECT_RCC, ENABLE);
   
    

	GPIO_InitStructure.GPIO_Pin   = SD_POWER_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SD_POWER_PORT, &GPIO_InitStructure);
	/* SD card power on. */
	GPIO_SetBits(SD_POWER_PORT, SD_POWER_Pin);

	GPIO_InitStructure.GPIO_Pin   = SD_DETECT_Pin;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
	GPIO_Init(SD_DETECT_PORT, &GPIO_InitStructure);


#if defined(RT_USING_W25QXX)
    w25qxx_init("w25q64","spi10");
#endif

#ifdef RT_USING_SD_CARD
    /* init sdcard driver */
    {
        extern void rt_hw_msd_init(void);
//        GPIO_InitTypeDef  GPIO_InitStructure;

        /* PA0 : SD Power */
//         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

//         GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
//         GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//         GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//         GPIO_Init(GPIOA, &GPIO_InitStructure);

//         /* SD card power on. */
//         GPIO_ResetBits(GPIOA, GPIO_Pin_0);
        rt_thread_delay(2);

        msd_init("sd0", "spi11");
    }
#endif /* RT_USING_DFS && RT_USING_DFS_ELMFAT */

#endif // RT_USING_SPI

#ifdef RT_USING_LWIP
    /* initialize eth interface */
    rt_hw_stm32_eth_init();
#endif /* RT_USING_LWIP */

}
