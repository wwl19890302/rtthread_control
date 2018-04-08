#include "adc.h"
#include "rthw.h"
#include "stm32f10x.h"
#include "stdio.h"

/*************************
ADC1_CH12 -->  PC2

*************************/
#define ADC_Pin         GPIO_Pin_2
#define ADC_PORT        GPIOC
#define ADC_PORT_RCC    RCC_APB2Periph_GPIOC
#define ADC_RCC         RCC_APB2Periph_ADC1
#define ADC_INDEX       ADC1
#define ADC_CH          12

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t   adc_stack[ 512 ];
static struct rt_thread adc_thread;

void ADC_VBAT_Init(void)
{
    ADC_InitTypeDef		ADC_InitStructure;
    GPIO_InitTypeDef	GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(ADC_PORT_RCC | ADC_RCC, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);		//72/6=12MHz,max:14MHz

    
    GPIO_InitStructure.GPIO_Pin = ADC_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//AI
    GPIO_Init(ADC_PORT,&GPIO_InitStructure);

    ADC_DeInit(ADC_INDEX);	//RESET ADC
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC	Independent
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;		//ONE CHANNEL
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ONE TIME
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//soft trigger
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC right align
    ADC_InitStructure.ADC_NbrOfChannel = 1;		//
    ADC_Init(ADC_INDEX,&ADC_InitStructure);			//

    ADC_Cmd(ADC_INDEX,ENABLE);	//enable adc
    ADC_ResetCalibration(ADC_INDEX);	//
    while(ADC_GetResetCalibrationStatus(ADC_INDEX));		//wait reset calibration ok
    ADC_StartCalibration(ADC_INDEX);	//
    while(ADC_GetCalibrationStatus(ADC_INDEX));			//wait calibration ok
}

/**************************
    get adc
    ADC1_chx
***************************/
rt_uint16_t Get_Adc(rt_uint8_t ch)
{
    
    ADC_RegularChannelConfig(ADC_INDEX,ch,1,ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC_INDEX,ENABLE);			
    while(!ADC_GetFlagStatus(ADC_INDEX,ADC_FLAG_EOC));	//wait convert ok
    return	ADC_GetConversionValue(ADC_INDEX);		//
}

rt_uint16_t	Get_Adc_Average(rt_uint8_t ch,rt_uint8_t times)
{
    rt_uint32_t	temp_val = 0;
    rt_uint8_t t;
    for(t=0;t<times;t++)
    {
        temp_val += Get_Adc(ch);
        rt_thread_delay(RT_TICK_PER_SECOND/200);
    }
    return	temp_val/times;
}

static void adc_thread_entry(void* parameter)
{
    float vbat = 0;
    rt_uint8_t s[5];

extern void spi_flash_test(void);
    ADC_VBAT_Init();
    while(1)
    {
        vbat = (float)Get_Adc_Average(ADC_CH, 5)*(1.03*25*3.3/4096);  //(float)adc_temp*(10*3.3/4096);  1.03->adjust
        sprintf(s,"%.2f",vbat);
        rt_kprintf("Vbat:%s\r\n",s);
        rt_thread_delay(RT_TICK_PER_SECOND*5);
//		spi_flash_test();
    }
}

int rt_adc_init(void)
{
    rt_err_t result;
    result = rt_thread_init(&adc_thread,
                            "adc",
                            adc_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&adc_stack[0],
                            sizeof(adc_stack),
                            16,15);
    if(result == RT_EOK)
    {
        rt_thread_startup(&adc_thread);
        return 0;
    }
    rt_kprintf("adc thread startup failed!");
    return -1;
}

