#include "uart5.h"
#include "rtthread.h"

//#define USING_RX_CRC_SUM

#define WL_433M_CMD_SIZE    10
#define WL_433M_RX_BEGIN    ('$')
#define WL_433M_RX_END      ('\r')

enum rx_stat
{
    WAIT_BEGIN,
    WAIT_END,
};

struct rt_uart_rx
{
    struct rt_semaphore rx_sem;
    enum rx_stat stat;
    uint8_t index;
    char rx_buffer[WL_433M_CMD_SIZE];
};

struct rt_uart_rx WL_433M_rx;

/* crc sum check */
uint8_t crc_sum(uint8_t *data, uint8_t length)
{
    uint8_t sum = 0;
    uint8_t i;

    for(i=0;i<length;i++)
    {
        sum += *data;
        data++;
    }
    return sum;
}

rt_err_t WL_433M_uart_input(rt_device_t dev, rt_size_t size)
{
    char ch;
    ch = 0;

    if(rt_device_read(dev, -1, &ch, 1) != 1)
        return -1;

    if(ch == WL_433M_RX_BEGIN)      //start one cmd
    {
        WL_433M_rx.stat = WAIT_END;
//        index = 0;
    }
    else if(WL_433M_rx.stat == WAIT_END)
    {
        if(ch == WL_433M_RX_END)        //receive one cmd end
        {
            WL_433M_rx.stat = WAIT_BEGIN;
            rt_sem_release(&WL_433M_rx.rx_sem);
        }
        else
        {
            WL_433M_rx.rx_buffer[WL_433M_rx.index] = ch;
            WL_433M_rx.index ++;
            if(WL_433M_rx.index >= (WL_433M_CMD_SIZE+2))
            {
                WL_433M_rx.stat = WAIT_BEGIN;
                WL_433M_rx.index = 0;
            }
        }
    }
    return RT_EOK;
}

void WL_433M_thread_entry(void * parameter)
{


    rt_device_t device;

    device = rt_device_find("uart5");
    if(device == RT_NULL)
    {
        rt_kprintf("WL_433M: can not find device: uart5\n");
        return;
    }

    if(rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("uart5: can not open device: uart5\n");
        return;
    }

    rt_device_set_rx_indicate(device, WL_433M_uart_input);

    while(1)
    {
        rt_sem_take(&WL_433M_rx.rx_sem, RT_WAITING_FOREVER);

#ifdef  USING_RX_CRC_SUM
        if(WL_433M_rx.rx_buffer[WL_433M_rx.index-1] == crc_sum(WL_433M_rx.rx_buffer, WL_433M_rx.index-1))
#endif
        {
            WL_433M_rx.rx_buffer[WL_433M_rx.index-1] = '\0';
            rt_kprintf("%s\r\n",WL_433M_rx.rx_buffer);
            rt_device_write(device, 0, WL_433M_rx.rx_buffer, WL_433M_rx.index-1);
            WL_433M_rx.index = 0;
        }

    }
}

void rt_wl_433m_init(void)
{
    rt_thread_t thread;

    rt_err_t result;

    rt_memset(&WL_433M_rx, 0, sizeof(struct rt_uart_rx));
    result = rt_sem_init(&WL_433M_rx.rx_sem, "wl_433_rx_sem", 0, RT_IPC_FLAG_FIFO);

    if(result != RT_EOK)
    {
        rt_kprintf("init message queue failed.\r\n");
        return;
    }

    thread = rt_thread_create("WL_433M", WL_433M_thread_entry, RT_NULL, 1024, 25, 7);
    if(thread != RT_NULL)
        rt_thread_startup(thread);
}
