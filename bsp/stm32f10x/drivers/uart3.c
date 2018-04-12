#include "uart3.h"
#include "rtthread.h"

//#define USING_RX_CRC_SUM

#define UART_DATA_CMD_SIZE    10
#define UART_DATA_RX_BEGIN    ('$')
#define UART_DATA_RX_END      ('\r')

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
    char rx_buffer[UART_DATA_CMD_SIZE];
};

struct rt_uart_rx UART_DATA_rx;

extern uint8_t crc_sum(uint8_t *data, uint8_t length);
// /* crc sum check */
// uint8_t crc_sum(uint8_t *data, uint8_t length)
// {
//     uint8_t sum = 0;
//     uint8_t i;

//     for(i=0;i<length;i++)
//     {
//         sum += *data;
//         data++;
//     }
//     return sum;
// }

rt_err_t UART_DATA_uart_input(rt_device_t dev, rt_size_t size)
{
    char ch;
    ch = 0;

    if(rt_device_read(dev, -1, &ch, 1) != 1)
        return -1;

    if(ch == UART_DATA_RX_BEGIN)      //start one cmd
    {
        UART_DATA_rx.stat = WAIT_END;
//        index = 0;
    }
    else if(UART_DATA_rx.stat == WAIT_END)
    {
        if(ch == UART_DATA_RX_END)        //receive one cmd end
        {
            UART_DATA_rx.stat = WAIT_BEGIN;
            rt_sem_release(&UART_DATA_rx.rx_sem);
        }
        else
        {
            UART_DATA_rx.rx_buffer[UART_DATA_rx.index] = ch;
            UART_DATA_rx.index ++;
            if(UART_DATA_rx.index >= (UART_DATA_CMD_SIZE+2))
            {
                UART_DATA_rx.stat = WAIT_BEGIN;
                UART_DATA_rx.index = 0;
            }
        }
    }
    return RT_EOK;
}

void UART_DATA_thread_entry(void * parameter)
{


    rt_device_t device;

    device = rt_device_find("uart3");
    if(device == RT_NULL)
    {
        rt_kprintf("UART_DATA: can not find device: uart5\n");
        return;
    }

    if(rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("uart5: can not open device: uart3\n");
        return;
    }

    rt_device_set_rx_indicate(device, UART_DATA_uart_input);

    while(1)
    {
        rt_sem_take(&UART_DATA_rx.rx_sem, RT_WAITING_FOREVER);

#ifdef  USING_RX_CRC_SUM
        if(UART_DATA_rx.rx_buffer[UART_DATA_rx.index-1] == crc_sum(UART_DATA_rx.rx_buffer, UART_DATA_rx.index-1))
#endif
        {
            UART_DATA_rx.rx_buffer[UART_DATA_rx.index-1] = '\0';
            rt_kprintf("%s\r\n",UART_DATA_rx.rx_buffer);
            rt_device_write(device, 0, UART_DATA_rx.rx_buffer, UART_DATA_rx.index-1);
            UART_DATA_rx.index = 0;
        }

    }
}

void rt_uart_data_init(void)
{
    rt_thread_t thread;

    rt_err_t result;

    rt_memset(&UART_DATA_rx, 0, sizeof(struct rt_uart_rx));
    result = rt_sem_init(&UART_DATA_rx.rx_sem, "wl_433_rx_sem", 0, RT_IPC_FLAG_FIFO);

    if(result != RT_EOK)
    {
        rt_kprintf("init message queue failed.\r\n");
        return;
    }

    thread = rt_thread_create("UART_DATA", UART_DATA_thread_entry, RT_NULL, 1024, 25, 7);
    if(thread != RT_NULL)
        rt_thread_startup(thread);
}
